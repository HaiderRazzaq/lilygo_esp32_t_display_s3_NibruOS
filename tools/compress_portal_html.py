import gzip
import os

def main():
    html_path = os.path.join('src', 'services', 'web_portal', 'index.html')
    out_path = os.path.join('src', 'services', 'WebPortalHtml.h')

    if not os.path.exists(html_path):
        print(f"Error: {html_path} does not exist!")
        return

    with open(html_path, 'r', encoding='utf-8') as f:
        html = f.read().encode('utf-8')
    
    gz = gzip.compress(html, compresslevel=9)
    print(f"Read {html_path}:")
    print(f"  Original size: {len(html)} bytes")
    print(f"  Gzip size:     {len(gz)} bytes ({len(gz)*100//len(html)}% of original)")

    with open(out_path, 'w', encoding='utf-8') as out:
        out.write("#pragma once\n")
        out.write("#include <stdint.h>\n")
        out.write("#if defined(ARDUINO)\n")
        out.write("#include <pgmspace.h>\n")
        out.write("#else\n")
        out.write("#ifndef PROGMEM\n")
        out.write("#define PROGMEM\n")
        out.write("#endif\n")
        out.write("#endif\n\n")
        out.write(f"// Auto-generated from src/services/web_portal/index.html\n")
        out.write(f"// Raw size: {len(html)} bytes -> Gzip compressed: {len(gz)} bytes\n")
        out.write(f"const uint8_t kIndexHtmlGz[{len(gz)}] PROGMEM = {{\n")
        for i, b in enumerate(gz):
            if i % 16 == 0:
                out.write("  ")
            out.write(f"0x{b:02x}, ")
            if i % 16 == 15 or i == len(gz) - 1:
                out.write("\n")
        out.write("};\n")
    print(f"Generated {out_path} successfully.")

if __name__ == '__main__':
    main()
