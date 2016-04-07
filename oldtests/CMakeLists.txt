# oldtests/CMakeLists.txt

add_executable(test_egm test_egm.cpp)
target_link_libraries(test_egm rocket)

add_executable(test_jpl test_jpl.cpp)
target_link_libraries(test_jpl rocket)

add_executable(test_sp3 test_sp3.cpp)
target_link_libraries(test_sp3 rocket)

add_executable(test_iers test_iers.cpp)
target_link_libraries(test_iers rocket)
