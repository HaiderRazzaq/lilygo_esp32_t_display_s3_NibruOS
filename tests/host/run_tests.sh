#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/../.."
# Keep engineering records consistent alongside the existing validation path.
"${PYTHON:-python}" tools/check_docs.py
mkdir -p .testbuild
c++ -std=c++17 -Wall -Wextra -Werror -Itests/host/stubs -Iinclude -Isrc \
  src/core/*.cpp src/hardware/*.cpp src/services/*.cpp src/apps/*.cpp \
  tests/host/test_main.cpp tests/host/test_settings.cpp -o .testbuild/nibru_smoke
# The real entry unit is separately compile-checked with mock Arduino headers.
c++ -std=c++17 -Wall -Wextra -Werror -Itests/host/stubs -Iinclude -Isrc \
  -c src/main.cpp -o .testbuild/main.o
.testbuild/nibru_smoke
