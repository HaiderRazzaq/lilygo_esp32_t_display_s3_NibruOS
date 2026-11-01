"""Windows compiler fallback for the shared SDL2 tests; never downloads dependencies.

Set CXX (for example '.testbuild/toolchain/ziglang/zig.exe c++') and SDL2_ROOT
(include/SDL.h, SDL2.lib, SDL2.dll). Local ignored test-toolchain paths are also
detected. Normal CMake/MSVC builds remain supported by run_desktop_tests.bat.
"""
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
local_zig = out / 'toolchain/ziglang/zig.exe'
compiler = (shlex.split(os.environ['CXX'], posix=False) if os.environ.get('CXX')
            else [str(local_zig), 'c++'] if local_zig.exists() else ['c++'])
sdl = Path(os.environ.get('SDL2_ROOT', out / 'sdl')).resolve()
if os.name != 'nt' or not (sdl / 'include/SDL.h').exists() or not (sdl / 'SDL2.lib').exists():
    sys.exit('This fallback needs Windows and SDL2_ROOT with include/SDL.h, SDL2.lib and SDL2.dll; otherwise use CMake.')
shutil.copyfile(sdl / 'SDL2.dll', out / 'SDL2.dll')
flags = ['-std=c++17', '-Wall', '-Wextra', '-Wno-unused-parameter', '-I.',
         '-DUNIT_TEST=1', '-DNIBRU_DESKTOP_SIMULATOR=1', '-DNIBRU_HEADLESS=1',
         '-Iplatform/desktop/stubs', '-Iplatform/desktop', '-Iinclude', '-Isrc', f'-I{sdl / "include"}']
if 'zig.exe' in compiler[0]:
    flags += ['-Wno-nullability-completeness']
sources = [str(p) for layer in ('core', 'hardware', 'ui', 'services', 'apps')
           for p in (ROOT / 'src' / layer).glob('*.cpp')]
sources += [str(p) for p in (ROOT / 'platform/desktop').glob('*.cpp') if p.name != 'DesktopMain.cpp']
entries = [('tests/desktop/test_simulator.cpp', 'auth_desktop_tests.exe')]
if '--smoke' in sys.argv:
    entries.append(('platform/desktop/DesktopMain.cpp', 'auth_simulator.exe'))
for entry, name in entries:
    subprocess.run(compiler + flags + sources + [entry, str(sdl / 'SDL2.lib'), '-lbcrypt', '-lpsapi', '-o', str(out / name)], check=True)
subprocess.run([str(out / 'auth_desktop_tests.exe')], check=True)
if '--smoke' in sys.argv:
    subprocess.run([str(out / 'auth_simulator.exe'), '--headless', '--test-frames=5', '--runtime-dir=.testbuild/auth_runtime'], check=True)
