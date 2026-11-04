import re

css_add = """\
.footer-links { display: flex; align-items: center; gap: 8px; flex-wrap: wrap; }
.footer-link { display: inline-flex; align-items: center; gap: 6px; font-size: 11.5px; color: #b8cde6; padding: 4px 10px; border-radius: 8px; background: rgba(56, 255, 225, .06); border: 1px solid rgba(56, 255, 225, .18); text-decoration: none; transition: 0.2s; }
.footer-link:hover { color: var(--brand-aqua); border-color: var(--brand-aqua); background: rgba(56, 255, 225, .14); }
.footer-link svg { flex-shrink: 0; fill: currentColor; }
.developer-credit a { color: #e8f5f7; font-weight: 700; text-decoration: underline; text-decoration-color: rgba(56, 255, 225, .4); }
.developer-credit a:hover { color: var(--brand-aqua); text-decoration-color: var(--brand-aqua); }
"""

html_footer_target = """<div class="developer-credit" dir="ltr">Developed by <strong>Haider Razaq</strong></div>"""

html_footer_replacement = """<div class="footer-links" dir="ltr">
<a href="https://www.linkedin.com/company/nibruiq/" target="_blank" rel="noopener noreferrer" class="footer-link">
<svg viewBox="0 0 24 24" width="13" height="13"><path d="M19 3a2 2 0 0 1 2 2v14a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h14m-.5 15.5v-5.3a3.26 3.26 0 0 0-3.26-3.26c-.85 0-1.84.52-2.28 1.3v-1.11h-2.79v8.37h2.79v-4.93c0-.77.62-1.4 1.39-1.4a1.4 1.4 0 0 1 1.4 1.4v4.93h2.75M6.46 10.9v8.37H9.2V10.9H6.46M7.83 6.64a1.64 1.64 0 1 0 0 3.28 1.64 1.64 0 0 0 0-3.28Z"/></svg>
<span>Nibru LinkedIn</span>
</a>
<a href="https://www.linkedin.com/in/haiderrazaq/" target="_blank" rel="noopener noreferrer" class="footer-link">
<svg viewBox="0 0 24 24" width="13" height="13"><path d="M19 3a2 2 0 0 1 2 2v14a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h14m-.5 15.5v-5.3a3.26 3.26 0 0 0-3.26-3.26c-.85 0-1.84.52-2.28 1.3v-1.11h-2.79v8.37h2.79v-4.93c0-.77.62-1.4 1.39-1.4a1.4 1.4 0 0 1 1.4 1.4v4.93h2.75M6.46 10.9v8.37H9.2V10.9H6.46M7.83 6.64a1.64 1.64 0 1 0 0 3.28 1.64 1.64 0 0 0 0-3.28Z"/></svg>
<span>Haider Razaq</span>
</a>
</div>
<div class="developer-credit" dir="ltr">Developed by <a href="https://www.linkedin.com/in/haiderrazaq/" target="_blank" rel="noopener noreferrer">Haider Razaq</a></div>"""

# 1. Update src/services/WebPortalService.cpp
with open('src/services/WebPortalService.cpp', 'r', encoding='utf-8') as f:
    wps = f.read()

# Add CSS before @media (max-width: 860px)
css_target = ".developer-credit strong { color: #e8f5f7; font-weight: 700; }"
if css_target in wps:
    wps = wps.replace(css_target, css_target + "\n" + css_add, 1)
    print("WebPortalService.cpp: CSS added")

if html_footer_target in wps:
    wps = wps.replace(html_footer_target, html_footer_replacement, 1)
    print("WebPortalService.cpp: Footer HTML added")

with open('src/services/WebPortalService.cpp', 'w', encoding='utf-8') as f:
    f.write(wps)

# 2. Update tools/account_setup/test_portal_preview.html
with open('tools/account_setup/test_portal_preview.html', 'r', encoding='utf-8') as f:
    preview = f.read()

if css_target in preview:
    preview = preview.replace(css_target, css_target + "\n" + css_add, 1)
    print("test_portal_preview.html: CSS added")

if html_footer_target in preview:
    preview = preview.replace(html_footer_target, html_footer_replacement, 1)
    print("test_portal_preview.html: Footer HTML added")

with open('tools/account_setup/test_portal_preview.html', 'w', encoding='utf-8') as f:
    f.write(preview)

# 3. Update tools/account_setup/account_setup.html
with open('tools/account_setup/account_setup.html', 'r', encoding='utf-8') as f:
    as_html = f.read()

if html_footer_target in as_html:
    as_html = as_html.replace(html_footer_target, html_footer_replacement, 1)
    print("account_setup.html: Footer HTML added")

with open('tools/account_setup/account_setup.html', 'w', encoding='utf-8') as f:
    f.write(as_html)

# 4. Update tools/account_setup/account_setup.css
with open('tools/account_setup/account_setup.css', 'r', encoding='utf-8') as f:
    as_css = f.read()

css_target_as = ".developer-credit strong { color: #e8f5f7; font-weight: 700; }"
if css_target_as in as_css:
    as_css = as_css.replace(css_target_as, css_target_as + "\n" + css_add, 1)
    print("account_setup.css: CSS added")

with open('tools/account_setup/account_setup.css', 'w', encoding='utf-8') as f:
    f.write(as_css)

print("All web pages updated successfully!")
