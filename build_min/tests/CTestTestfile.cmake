# CMake generated Testfile for 
# Source directory: /Users/abossard/Desktop/projects/qtui-test-demo/tests
# Build directory: /Users/abossard/Desktop/projects/qtui-test-demo/build_min/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[feature_flight_console]=] "/Users/abossard/Desktop/projects/qtui-test-demo/build_min/tests/feature_flight_console")
set_tests_properties([=[feature_flight_console]=] PROPERTIES  _BACKTRACE_TRIPLES "/Users/abossard/Desktop/projects/qtui-test-demo/tests/CMakeLists.txt;27;add_test;/Users/abossard/Desktop/projects/qtui-test-demo/tests/CMakeLists.txt;0;")
add_test([=[unit_simulation_physics]=] "/Users/abossard/Desktop/projects/qtui-test-demo/build_min/tests/unit_simulation_physics")
set_tests_properties([=[unit_simulation_physics]=] PROPERTIES  _BACKTRACE_TRIPLES "/Users/abossard/Desktop/projects/qtui-test-demo/tests/CMakeLists.txt;35;add_test;/Users/abossard/Desktop/projects/qtui-test-demo/tests/CMakeLists.txt;0;")
add_test([=[unit_low_fuel_alert]=] "/Users/abossard/Desktop/projects/qtui-test-demo/build_min/tests/unit_low_fuel_alert")
set_tests_properties([=[unit_low_fuel_alert]=] PROPERTIES  _BACKTRACE_TRIPLES "/Users/abossard/Desktop/projects/qtui-test-demo/tests/CMakeLists.txt;43;add_test;/Users/abossard/Desktop/projects/qtui-test-demo/tests/CMakeLists.txt;0;")
add_test([=[unit_stability_index]=] "/Users/abossard/Desktop/projects/qtui-test-demo/build_min/tests/unit_stability_index")
set_tests_properties([=[unit_stability_index]=] PROPERTIES  _BACKTRACE_TRIPLES "/Users/abossard/Desktop/projects/qtui-test-demo/tests/CMakeLists.txt;51;add_test;/Users/abossard/Desktop/projects/qtui-test-demo/tests/CMakeLists.txt;0;")
add_test([=[unit_power_distribution]=] "/Users/abossard/Desktop/projects/qtui-test-demo/build_min/tests/unit_power_distribution")
set_tests_properties([=[unit_power_distribution]=] PROPERTIES  _BACKTRACE_TRIPLES "/Users/abossard/Desktop/projects/qtui-test-demo/tests/CMakeLists.txt;59;add_test;/Users/abossard/Desktop/projects/qtui-test-demo/tests/CMakeLists.txt;0;")
add_test([=[unit_theme_service]=] "/Users/abossard/Desktop/projects/qtui-test-demo/build_min/tests/unit_theme_service")
set_tests_properties([=[unit_theme_service]=] PROPERTIES  _BACKTRACE_TRIPLES "/Users/abossard/Desktop/projects/qtui-test-demo/tests/CMakeLists.txt;67;add_test;/Users/abossard/Desktop/projects/qtui-test-demo/tests/CMakeLists.txt;0;")
add_test([=[compile_smoke]=] "/Users/abossard/Desktop/projects/qtui-test-demo/build_min/tests/compile_smoke")
set_tests_properties([=[compile_smoke]=] PROPERTIES  _BACKTRACE_TRIPLES "/Users/abossard/Desktop/projects/qtui-test-demo/tests/CMakeLists.txt;74;add_test;/Users/abossard/Desktop/projects/qtui-test-demo/tests/CMakeLists.txt;0;")
subdirs("../_deps/googletest-build")
