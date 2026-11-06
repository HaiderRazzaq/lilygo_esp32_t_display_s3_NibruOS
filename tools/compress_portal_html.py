import gzip

def main():
    with open('src/services/WebPortalService.cpp', 'r', encoding='utf-8') as f:
        text = f.read()
    
    s_marker = 'R"h0('
    e_marker = ')h0"'
    i1 = text.find(s_marker)
    if i1 == -1:
        print("Start marker not found")
        return
    i1 += len(s_marker)
    i2 = text.find(e_marker, i1)
    if i2 == -1:
        print("End marker not found")
        return
    
    html = text[i1:i2].encode('utf-8')
    gz = gzip.compress(html, compresslevel=9)
    print(f"Original size: {len(html)} bytes")
    print(f"Gzip size:     {len(gz)} bytes")

    # Generate C byte array
    with open('src/services/WebPortalHtml.h', 'w', encoding='utf-8') as out:
        out.write("#pragma once\n")
        out.write("#include <stdint.h>\n")
        out.write("#if defined(ARDUINO)\n")
        out.write("#include <pgmspace.h>\n")
        out.write("#else\n")
        out.write("#ifndef PROGMEM\n")
        out.write("#define PROGMEM\n")
        out.write("#endif\n")
        out.write("#endif\n\n")
        out.write(f"// Gzip-compressed Web Portal Dashboard ({len(html)} bytes uncompressed -> {len(gz)} bytes gzipped)\n")
        out.write(f"const uint8_t kIndexHtmlGz[{len(gz)}] PROGMEM = {{\n")
        for i, b in enumerate(gz):
            if i % 16 == 0:
                out.write("  ")
            out.write(f"0x{b:02x}, ")
            if i % 16 == 15 or i == len(gz) - 1:
                out.write("\n")
        out.write("};\n")
    print("Generated src/services/WebPortalHtml.h successfully.")

if __name__ == '__main__':
    main()

