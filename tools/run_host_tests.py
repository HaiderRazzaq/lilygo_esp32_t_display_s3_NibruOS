"""Run mock C++ tests on Windows/POSIX. CXX may be 'path/to/zig.exe c++'."""
import os
from pathlib import Path
import shlex
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
os.chdir(ROOT)
out = ROOT / '.testbuild'
out.mkdir(exist_ok=True)
compiler = shlex.split(os.environ.get('CXX', 'c++'), posix=os.name != 'nt')
flags = ['-std=c++17', '-Wall', '-Wextra', '-Werror', '-Itests/host/stubs', '-Iinclude', '-Isrc']
flags += sys.argv[1:]
if 'zig.exe' in compiler[0]:
    flags += ['-Wno-nullability-completeness']
sources = [str(p) for d in ('core', 'hardware', 'services', 'apps') for p in (ROOT / 'src' / d).glob('*.cpp')]
exe = str(out / ('nibru_smoke.exe' if os.name == 'nt' else 'nibru_smoke'))
subprocess.run(compiler + flags + sources + ['tests/host/test_main.cpp', 'tests/host/test_settings.cpp', '-o', exe], check=True)
subprocess.run(compiler + flags + ['-c', 'src/main.cpp', '-o', str(out / 'main.o')], check=True)
subprocess.run([exe], check=True)
