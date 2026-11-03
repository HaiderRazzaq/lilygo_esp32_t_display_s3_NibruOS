"""Run mock C++ tests on Windows/POSIX. CXX may be 'path/to/zig.exe c++' or MSVC."""
import os
from pathlib import Path
import shlex
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
os.chdir(ROOT)
out = ROOT / '.testbuild'
out.mkdir(exist_ok=True)

cxx_env = os.environ.get('CXX')
if not cxx_env and os.name == 'nt' and not shutil.which('c++'):
    vcvars = Path(r"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat")
    if not vcvars.exists():
        vcvars = Path(r"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat")
    if vcvars.exists():
        # Compile using MSVC cl.exe
        sources = [str(p) for d in ('core', 'hardware', 'ui', 'services', 'apps') for p in (ROOT / 'src' / d).glob('*.cpp')]
        test_sources = ['tests/host/test_main.cpp', 'tests/host/test_settings.cpp', 'tests/host/test_authenticator.cpp', 'tests/host/test_account_import.cpp', 'tests/host/test_vault.cpp', 'tests/host/test_web_portal.cpp']
        exe = str(out / 'nibru_smoke.exe')
        cl_cmd = f'call "{vcvars}" x64 >nul 2>&1 && cl /nologo /std:c++17 /EHsc /utf-8 /W4 /wd4996 /wd4267 /wd4244 /wd4100 /I. /Itests/host/stubs /Iinclude /Isrc {" ".join(sources)} {" ".join(test_sources)} bcrypt.lib /Fe"{exe}" /Fo"{out}/"'
        subprocess.run(cl_cmd, shell=True, check=True)
        # Verify compile-only on main.cpp
        main_obj = str(out / 'main.obj')
        cl_main = f'call "{vcvars}" x64 >nul 2>&1 && cl /nologo /std:c++17 /EHsc /utf-8 /W4 /wd4996 /wd4267 /wd4244 /wd4100 /I. /Itests/host/stubs /Iinclude /Isrc /c src/main.cpp /Fo"{main_obj}"'
        subprocess.run(cl_main, shell=True, check=True)
        subprocess.run([exe], check=True)
        sys.exit(0)

compiler = shlex.split(cxx_env or 'c++', posix=os.name != 'nt')
flags = ['-std=c++17', '-Wall', '-Wextra', '-Werror', '-I.', '-Itests/host/stubs', '-Iinclude', '-Isrc']
flags += sys.argv[1:]
if 'zig.exe' in compiler[0]:
    flags += ['-Wno-nullability-completeness']
sources = [str(p) for d in ('core', 'hardware', 'ui', 'services', 'apps') for p in (ROOT / 'src' / d).glob('*.cpp')]
exe = str(out / ('nibru_smoke.exe' if os.name == 'nt' else 'nibru_smoke'))
subprocess.run(compiler + flags + sources + ['tests/host/test_main.cpp', 'tests/host/test_settings.cpp', 'tests/host/test_authenticator.cpp', 'tests/host/test_account_import.cpp', '-o', exe] + (['-lbcrypt'] if os.name == 'nt' else []), check=True)
subprocess.run(compiler + flags + ['-c', 'src/main.cpp', '-o', str(out / 'main.o')], check=True)
subprocess.run([exe], check=True)
