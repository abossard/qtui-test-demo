# Tests CMake configuration (Phase 1: Setup) - Tasks T002/T003
cmake_minimum_required(VERSION 3.24)

include(FetchContent)

# GoogleTest (v1.14.0 for C++23 support maturity)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Placeholder smoke test target (will be replaced / expanded in Phase 2)
add_executable(compile_smoke unit/compile_smoke.cpp)
# Use main from GTest for simplicity
target_link_libraries(compile_smoke PRIVATE GTest::gtest_main)

# Ensure consistent standard (inherit from parent policy if available)
set_property(TARGET compile_smoke PROPERTY CXX_STANDARD 23)
set_property(TARGET compile_smoke PROPERTY CXX_STANDARD_REQUIRED ON)

add_test(NAME compile_smoke COMMAND compile_smoke)
