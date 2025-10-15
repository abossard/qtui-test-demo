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
target_link_libraries(compile_smoke PRIVATE GTest::gtest_main)

if (TARGET space_cockpit_core)
  add_executable(feature_flight_console
    feature/test_flight_console_behavior.cpp
  )
  # Use gtest_main to supply the test runner entry point (fixes missing _main linker error)
  target_link_libraries(feature_flight_console PRIVATE GTest::gtest_main Qt6::Core space_cockpit_core)
  set_property(TARGET feature_flight_console PROPERTY CXX_STANDARD 23)
  add_test(NAME feature_flight_console COMMAND feature_flight_console)
  
  # Unit physics red test (T021) – will fail until physics implemented
  add_executable(unit_simulation_physics
    unit/test_simulation_physics.cpp
  )
  target_link_libraries(unit_simulation_physics PRIVATE GTest::gtest_main Qt6::Core space_cockpit_core)
  set_property(TARGET unit_simulation_physics PROPERTY CXX_STANDARD 23)
  add_test(NAME unit_simulation_physics COMMAND unit_simulation_physics)

  # Low fuel alert test
  add_executable(unit_low_fuel_alert
    unit/test_low_fuel_alert.cpp
  )
  target_link_libraries(unit_low_fuel_alert PRIVATE GTest::gtest_main Qt6::Core space_cockpit_core)
  set_property(TARGET unit_low_fuel_alert PROPERTY CXX_STANDARD 23)
  add_test(NAME unit_low_fuel_alert COMMAND unit_low_fuel_alert)

  # Stability index tests
  add_executable(unit_stability_index
    unit/test_stability_index.cpp
  )
  target_link_libraries(unit_stability_index PRIVATE GTest::gtest_main Qt6::Core space_cockpit_core)
  set_property(TARGET unit_stability_index PROPERTY CXX_STANDARD 23)
  add_test(NAME unit_stability_index COMMAND unit_stability_index)

  # Power distribution tests
  add_executable(unit_power_distribution
    unit/test_power_distribution.cpp
  )
  target_link_libraries(unit_power_distribution PRIVATE GTest::gtest_main Qt6::Core space_cockpit_core)
  set_property(TARGET unit_power_distribution PROPERTY CXX_STANDARD 23)
  add_test(NAME unit_power_distribution COMMAND unit_power_distribution)

  # Theme service tests
  add_executable(unit_theme_service
    unit/test_theme_service.cpp
  )
  target_link_libraries(unit_theme_service PRIVATE GTest::gtest_main Qt6::Core space_cockpit_core)
  set_property(TARGET unit_theme_service PROPERTY CXX_STANDARD 23)
  add_test(NAME unit_theme_service COMMAND unit_theme_service)
endif()

# Ensure consistent standard (inherit from parent policy if available)
set_property(TARGET compile_smoke PROPERTY CXX_STANDARD 23)
set_property(TARGET compile_smoke PROPERTY CXX_STANDARD_REQUIRED ON)

add_test(NAME compile_smoke COMMAND compile_smoke)
