# oldtests/CMakeLists.txt

add_executable(test_iers test_iers.cpp)
target_link_libraries(test_iers rocket)

add_executable(test_eop test_eop.cpp)
target_link_libraries(test_eop rocket)

add_executable(test_oi test_oi.cpp)
target_link_libraries(test_oi rocket)

add_executable(test_orbit_fit test_orbit_fit.cpp)
target_link_libraries(test_orbit_fit rocket)

add_executable(test_gds_serialize test_gds_serialize.cpp)
target_link_libraries(test_gds_serialize rocket)

