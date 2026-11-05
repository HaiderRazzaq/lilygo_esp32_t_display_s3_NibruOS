cmake_minimum_required(VERSION 3.16)
project(NibruOS_Simulator C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  add_compile_options(/W4 /utf-8 /wd4996)
else()
  add_compile_options(-Wall -Wextra -Wno-unused-parameter)
endif()

# SDL2 Discovery or Automatic FetchContent
set(SDL2_FOUND_LOCAL FALSE)
find_package(SDL2 CONFIG QUIET)

if(TARGET SDL2::SDL2)
  set(SDL2_TARGET SDL2::SDL2)
  set(SDL2_FOUND_LOCAL TRUE)
  message(STATUS "Found installed SDL2 package")
endif()

if(NOT SDL2_FOUND_LOCAL)
  include(FetchContent)
  if(WIN32 AND MSVC)
    message(STATUS "Fetching prebuilt official SDL2 VC Development Package...")
    FetchContent_Declare(
      SDL2_Prebuilt
      URL https://github.com/libsdl-org/SDL/releases/download/release-2.30.10/SDL2-devel-2.30.10-VC.zip
    )
    FetchContent_MakeAvailable(SDL2_Prebuilt)

    set(SDL2_ROOT_DIR "${sdl2_prebuilt_SOURCE_DIR}")
    if(NOT EXISTS "${SDL2_ROOT_DIR}/include/SDL.h")
      # Archive extracted into SDL2-2.30.10 subfolder
      file(GLOB SDL2_SUBDIR "${sdl2_prebuilt_SOURCE_DIR}/SDL2-*")
      if(SDL2_SUBDIR)
        set(SDL2_ROOT_DIR "${SDL2_SUBDIR}")
      endif()
    endif()

    set(SDL2_INCLUDE_DIR "${SDL2_ROOT_DIR}/include")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(SDL2_LIB_DIR "${SDL2_ROOT_DIR}/lib/x64")
    else()
      set(SDL2_LIB_DIR "${SDL2_ROOT_DIR}/lib/x86")
    endif()

    add_library(SDL2::SDL2 UNKNOWN IMPORTED)
    set_target_properties(SDL2::SDL2 PROPERTIES
      IMPORTED_LOCATION "${SDL2_LIB_DIR}/SDL2.lib"
      INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIR}"
    )

    set(SDL2_DLL_PATH "${SDL2_LIB_DIR}/SDL2.dll")
    set(SDL2_TARGET SDL2::SDL2)
  else()
    # On Linux/macOS or MinGW, fetch and build from source or use pkg-config
    FetchContent_Declare(
      SDL2
      GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
      GIT_TAG release-2.30.10
    )
    set(SDL_SHARED ON CACHE BOOL "" FORCE)
    set(SDL_STATIC OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(SDL2)
    set(SDL2_TARGET SDL2::SDL2)
  endif()
endif()

# Shared NibruOS sources
set(NIBRU_SHARED_SOURCES
  src/core/AppManager.cpp
  src/core/ResourceManager.cpp
  src/core/MemoryProfiler.cpp
  src/hardware/DisplayManager.cpp
  src/hardware/TouchManager.cpp
  src/hardware/ButtonManager.cpp
  src/ui/HomeScreen.cpp
  src/services/WiFiService.cpp
  src/services/SettingsService.cpp
  src/services/SecurityService.cpp
  src/services/TotpService.cpp
  src/services/AccountImportService.cpp
  src/services/CryptoService.cpp
  src/services/UsbHidService.cpp
  src/services/TimeService.cpp
  src/services/ArabicReshaper.cpp
  src/services/LocalizationService.cpp
  src/services/VaultService.cpp
  src/services/WebPortalService.cpp
  src/services/BleHidService.cpp
  src/services/WifiDefenseService.cpp
  src/apps/WiFiApp.cpp
  src/apps/WiFiChannelsApp.cpp
  src/apps/WifiDefenseApp.cpp
  src/apps/BluetoothApp.cpp
  src/apps/BeaconLabApp.cpp
  src/apps/WiFiScannerApp.cpp
  src/apps/TotpApp.cpp
  src/apps/VaultApp.cpp
  src/apps/SettingsApp.cpp
  src/apps/SystemInfoApp.cpp
  src/apps/TouchTestApp.cpp
  src/apps/GeneratorApp.cpp
  src/apps/ClockApp.cpp
)

# Desktop platform sources
set(NIBRU_DESKTOP_SOURCES
  platform/desktop/SDLDisplay.cpp
  platform/desktop/DesktopInput.cpp
  platform/desktop/MockWiFiService.cpp
  platform/desktop/DesktopStorage.cpp
  platform/desktop/DesktopDiagnostics.cpp
)

# Target: Desktop Simulator Executable
add_executable(nibru_simulator
  platform/desktop/DesktopMain.cpp
  ${NIBRU_SHARED_SOURCES}
  ${NIBRU_DESKTOP_SOURCES}
)

target_compile_definitions(nibru_simulator PRIVATE
  UNIT_TEST=1
  NIBRU_DESKTOP_SIMULATOR=1
)

target_include_directories(nibru_simulator PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  platform/desktop/stubs
  platform/desktop
  include
  src
)

target_link_libraries(nibru_simulator PRIVATE
  ${SDL2_TARGET}
)
if(WIN32)
  target_link_libraries(nibru_simulator PRIVATE bcrypt)
endif()

# Target: Desktop Automated Test Runner
add_executable(nibru_tests
  tests/desktop/test_simulator.cpp
  ${NIBRU_SHARED_SOURCES}
  ${NIBRU_DESKTOP_SOURCES}
)

target_compile_definitions(nibru_tests PRIVATE
  UNIT_TEST=1
  NIBRU_DESKTOP_SIMULATOR=1
  NIBRU_HEADLESS=1
)

target_include_directories(nibru_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  platform/desktop/stubs
  platform/desktop
  include
  src
)

target_link_libraries(nibru_tests PRIVATE
  ${SDL2_TARGET}
)
if(WIN32)
  target_link_libraries(nibru_tests PRIVATE bcrypt)
endif()

# Copy SDL2.dll next to output binaries on Windows
if(WIN32 AND DEFINED SDL2_DLL_PATH)
  add_custom_command(TARGET nibru_simulator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${SDL2_DLL_PATH}"
    $<TARGET_FILE_DIR:nibru_simulator>
    COMMENT "Copying SDL2.dll to binary directory"
  )
  add_custom_command(TARGET nibru_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${SDL2_DLL_PATH}"
    $<TARGET_FILE_DIR:nibru_tests>
    COMMENT "Copying SDL2.dll to test binary directory"
  )
endif()

enable_testing()
add_test(NAME DesktopTests COMMAND nibru_tests)
