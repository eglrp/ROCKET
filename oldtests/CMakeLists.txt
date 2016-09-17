# oldtests/CMakeLists.txt

add_executable(test_egm test_egm.cpp)
target_link_libraries(test_egm rocket)

add_executable(test_srp test_srp.cpp)
target_link_libraries(test_srp rocket)

add_executable(test_sp3 test_sp3.cpp)
target_link_libraries(test_sp3 rocket)

add_executable(test_iers test_iers.cpp)
target_link_libraries(test_iers rocket)

add_executable(test_eop test_eop.cpp)
target_link_libraries(test_eop rocket)

add_executable(test_rkf test_rkf.cpp)
target_link_libraries(test_rkf rocket)

add_executable(test_force test_force.cpp)
target_link_libraries(test_force rocket)

add_executable(test_oi test_oi.cpp)
target_link_libraries(test_oi rocket)

add_executable(test_jpl test_jpl.cpp)
target_link_libraries(test_jpl rocket)

add_executable(test_thirdbody test_thirdbody.cpp)
target_link_libraries(test_thirdbody rocket)

add_executable(test_legendre test_legendre.cpp)
target_link_libraries(test_legendre rocket)

add_executable(test_tides test_tides.cpp)
target_link_libraries(test_tides rocket)

add_executable(test_relativity test_relativity.cpp)
target_link_libraries(test_relativity rocket)

add_executable(test_gds_serialize test_gds_serialize.cpp)
target_link_libraries(test_gds_serialize rocket)

