import http.client
import importlib.util
import json
from pathlib import Path
import re
import threading
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[2]
spec = importlib.util.spec_from_file_location('account_setup', ROOT / 'tools/account_setup.py')
setup = importlib.util.module_from_spec(spec)
spec.loader.exec_module(setup)
FRAME = 'NI1:' + '00' * 12 + ':' + 'aa' * 8 + ':' + 'bb' * 16 + '\n'

class SetupTests(unittest.TestCase):
    def setUp(self):
        self.server = setup.create_server(0)
        self.thread = threading.Thread(target=self.server.serve_forever, daemon=True)
        self.thread.start()
        self.origin = 'http://127.0.0.1:' + str(self.server.server_port)
    def tearDown(self):
        self.server.shutdown(); self.server.server_close(); self.thread.join()
    def request(self, method, path, body=None, headers=None):
        connection = http.client.HTTPConnection('127.0.0.1', self.server.server_port, timeout=3)
        connection.request(method, path, body=body, headers=headers or {})
        response = connection.getresponse()
        result = response.status, dict(response.getheaders()), response.read()
        connection.close()
        return result
    def token(self):
        code, headers, html = self.request('GET', '/')
        self.assertEqual(code, 200)
        self.assertEqual(headers['Cache-Control'], 'no-store')
        self.assertIn("frame-ancestors 'none'", headers['Content-Security-Policy'])
        return re.search(rb'name="csrf" content="([a-f0-9]{48})"', html)[1].decode()
    def test_no_remote_host_or_paths(self):
        self.assertEqual(self.request('GET', '/', headers={'Host': 'attacker.example'})[0], 403)
        self.assertEqual(self.request('GET', '/../../AGENTS.md')[0], 404)
    def test_csrf_and_origin_required(self):
        self.assertEqual(self.request('POST', '/relay', '{}', {'Content-Type': 'application/json'})[0], 403)
        headers = {'Content-Type': 'application/json', 'Origin': self.origin, 'X-Nibru-CSRF': '\xe9'}
        self.assertEqual(self.request('POST', '/relay', '{}', headers)[0], 403)
    def test_ciphertext_only_relay(self):
        headers = {'Content-Type': 'application/json', 'Origin': self.origin, 'X-Nibru-CSRF': self.token()}
        with patch.object(setup, 'send_ciphertext') as send:
            body = json.dumps({'ip': '192.168.1.20', 'frame': FRAME})
            self.assertEqual(self.request('POST', '/relay', body, headers)[0], 200)
            send.assert_called_once_with('192.168.1.20', FRAME.encode('ascii'))
            for data in ({'ip': '8.8.8.8', 'frame': FRAME}, {'ip': '127.0.0.1', 'frame': FRAME},
                         {'ip': '192.168.1.20', 'secret': 'public-fixture'}, {'ip': '192.168.1.20', 'frame': 'plaintext'}):
                self.assertEqual(self.request('POST', '/relay', json.dumps(data), headers)[0], 400)
            self.assertEqual(send.call_count, 1)
    def test_oversize_is_rejected(self):
        headers = {'Content-Type': 'application/json', 'Origin': self.origin, 'X-Nibru-CSRF': self.token()}
        self.assertEqual(self.request('POST', '/relay', 'a' * 2049, headers)[0], 400)

if __name__ == '__main__':
    unittest.main()
