"""Trusted localhost companion: serve static UI and relay ONLY ciphertext over UDP.

No third-party dependencies, account files, body logging, cloud calls or device
HTTP server. Device must explicitly enter an unlocked paired import session.
"""
import argparse
import hmac
from http.server import BaseHTTPRequestHandler, HTTPServer
import ipaddress
import json
from pathlib import Path
import re
import secrets
import socket
import webbrowser

ASSETS = Path(__file__).resolve().parent / 'account_setup'
LAN = tuple(ipaddress.ip_network(n) for n in ('10.0.0.0/8', '172.16.0.0/12', '192.168.0.0/16'))
FRAME = re.compile(r'NI1:[0-9a-fA-F]{24}:(?:[0-9a-fA-F]{2}){1,96}:[0-9a-fA-F]{32}\n\Z')

def validate_relay(data):
    if not isinstance(data, dict) or set(data) != {'ip', 'frame'}:
        raise ValueError('Invalid request')
    address = ipaddress.IPv4Address(data['ip'])
    if not any(address in network for network in LAN) or address.is_multicast:
        raise ValueError('Use a local private IPv4 address')
    frame = data['frame']
    if not isinstance(frame, str) or not FRAME.fullmatch(frame):
        raise ValueError('Invalid encrypted envelope')
    return str(address), frame.encode('ascii')

def send_ciphertext(address, frame):
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as udp:
        udp.settimeout(.5)
        udp.sendto(frame, (address, 8766))

def create_server(port=8765):
    token = secrets.token_hex(24)
    class Handler(BaseHTTPRequestHandler):
        def log_message(self, *_):
            pass # Never persist request targets, fields, pairing codes or bodies.
        def setup(self):
            super().setup()
            self.connection.settimeout(2)
        def valid_host(self):
            return self.headers.get('Host') == '127.0.0.1:' + str(self.server.server_port)
        def reply(self, code, body=b'', mime='text/plain; charset=utf-8'):
            self.send_response(code)
            self.send_header('Content-Type', mime)
            self.send_header('Content-Length', str(len(body)))
            self.send_header('Cache-Control', 'no-store')
            self.send_header('X-Content-Type-Options', 'nosniff')
            self.send_header('Referrer-Policy', 'no-referrer')
            self.send_header('Content-Security-Policy', "default-src 'none'; script-src 'self'; style-src 'self'; connect-src 'self'; base-uri 'none'; form-action 'none'; frame-ancestors 'none'")
            self.end_headers()
            self.wfile.write(body)
        def do_GET(self):
            if not self.valid_host():
                self.reply(403); return
            assets = {'/': ('account_setup.html', 'text/html; charset=utf-8'),
                      '/account_setup.mjs': ('account_setup.mjs', 'text/javascript; charset=utf-8'),
                      '/account_setup.css': ('account_setup.css', 'text/css; charset=utf-8')}
            if self.path not in assets:
                self.reply(404); return
            file, mime = assets[self.path]
            body = (ASSETS / file).read_bytes()
            if self.path == '/':
                body = body.replace(b'__CSRF__', token.encode('ascii'))
            self.reply(200, body, mime)
        def do_POST(self):
            origin = 'http://127.0.0.1:' + str(self.server.server_port)
            provided = self.headers.get('X-Nibru-CSRF', '')
            if not self.valid_host() or self.headers.get('Origin') != origin or not re.fullmatch(r'[0-9a-f]{48}', provided) or not hmac.compare_digest(provided, token):
                self.reply(403); return
            if self.path != '/relay' or self.headers.get('Content-Type') != 'application/json':
                self.reply(400); return
            try:
                length = int(self.headers.get('Content-Length', '0'))
                if not 0 < length <= 2048:
                    raise ValueError()
                address, frame = validate_relay(json.loads(self.rfile.read(length)))
                send_ciphertext(address, frame)
            except (ValueError, TypeError, OSError):
                self.reply(400, b'Relay rejected'); return
            self.reply(200, b'{"queued":true}', 'application/json')
    return HTTPServer(('127.0.0.1', port), Handler)

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--port', type=int, default=8765)
    parser.add_argument('--no-browser', action='store_true')
    args = parser.parse_args()
    server = create_server(args.port)
    url = 'http://127.0.0.1:' + str(server.server_port)
    print('NibruOS Account Setup: ' + url)
    if not args.no_browser:
        webbrowser.open(url)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        server.server_close()

if __name__ == '__main__':
    main()
