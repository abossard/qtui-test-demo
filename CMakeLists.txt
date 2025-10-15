cmake_minimum_required(VERSION 3.24)

project(SpaceCockpitDemo LANGUAGES CXX)

# --- Testing framework bootstrap (must be before add_subdirectory(tests)) ---
# Historically we placed enable_testing() inside tests/CMakeLists.txt, which causes
# CTest to generate a tests/CTestTestfile.cmake but NOT a top-level CTestTestfile.cmake.
# Some invocations like `ctest --test-dir build --output-on-failure` rely on the
# presence of the top-level test file; without it you may see "No tests were found!!!".
# Calling include(CTest) + enable_testing() here ensures top-level discovery.
include(CTest)            # Defines BUILD_TESTING option if not already defined
if (BUILD_TESTING)
  enable_testing()
endif()

option(ENABLE_SPACE_COCKPIT "Build Space Cockpit Kitchensink feature" ON)

# Enforce C++23 (fallback to 20 with warning if unsupported)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Detect compiler feature support; fallback path
if (NOT "cxx_std_23" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
  message(WARNING "Compiler does not advertise cxx_std_23; falling back to C++20 (feature guard remains).")
  set(CMAKE_CXX_STANDARD 20)
endif()

# Common warnings (extend later with per-compiler granularity)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options(-Wall -Wextra -Wpedantic)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options(/W4)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if (ENABLE_SPACE_COCKPIT)
  find_package(Qt6 6.8 REQUIRED COMPONENTS Core Widgets Qml OPTIONAL_COMPONENTS Gui)

  # Core library (Phase 2 foundational stubs)
  add_library(space_cockpit_core
    src/core/types.h
    src/core/SimulationCore.h
    src/core/SimulationCore.cpp
    src/core/StabilityCalculator.h
    src/core/StabilityCalculator.cpp
    src/core/Log.h
    src/core/ThresholdConfig.h
    src/services/MetricsRecorder.h src/services/MetricsRecorder.cpp
    src/services/ThemeService.h src/services/ThemeService.cpp
    src/services/AlertBus.h src/services/AlertBus.cpp
    src/services/ReplayService.h src/services/ReplayService.cpp
    src/services/ScriptSandbox.h src/services/ScriptSandbox.cpp
    src/services/PowerDistributionService.h
    src/adapters/SimulationController.h src/adapters/SimulationController.cpp
  )
  # Add Qt6::Qml for QJSEngine (ScriptSandbox)
  target_link_libraries(space_cockpit_core PUBLIC Qt6::Core Qt6::Qml Qt6::Gui)
  target_compile_features(space_cockpit_core PUBLIC cxx_std_23)
  target_include_directories(space_cockpit_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

  add_executable(space_cockpit
    src/ui/main.cpp
    src/ui/FlightConsolePanel.h
    src/ui/FlightConsolePanel.cpp
    src/ui/PowerPanel.h
    src/ui/PowerPanel.cpp
  )
  target_link_libraries(space_cockpit PRIVATE space_cockpit_core Qt6::Core Qt6::Widgets Qt6::Qml Qt6::Gui)
  target_compile_definitions(space_cockpit PRIVATE ENABLE_SPACE_COCKPIT=1)
  target_compile_features(space_cockpit PRIVATE cxx_std_23)
endif()

# Tests subdirectory (always add so CI sees test infra even if feature toggle off)
add_subdirectory(tests)

message(STATUS "Configure Summary:")
message(STATUS "  ENABLE_SPACE_COCKPIT = ${ENABLE_SPACE_COCKPIT}")
message(STATUS "  C++ Standard        = ${CMAKE_CXX_STANDARD}")
message(STATUS "  BUILD_TESTING       = ${BUILD_TESTING}")
