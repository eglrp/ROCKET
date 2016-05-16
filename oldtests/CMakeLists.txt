# oldtests/CMakeLists.txt

add_executable(test_egm test_egm.cpp)
target_link_libraries(test_egm rocket)

add_executable(test_srp test_srp.cpp)
target_link_libraries(test_srp rocket)

add_executable(test_sp3 test_sp3.cpp)
target_link_libraries(test_sp3 rocket)

add_executable(test_iers test_iers.cpp)
target_link_libraries(test_iers rocket)

add_executable(test_de405 test_de405.cpp)
target_link_libraries(test_de405 rocket)

add_executable(test_integrator test_integrator.cpp)
target_link_libraries(test_integrator rocket)

add_executable(test_oi test_oi.cpp)
target_link_libraries(test_oi rocket)

add_executable(test_inp test_inp.cpp)
target_link_libraries(test_inp rocket)

add_executable(test_jpl test_jpl.cpp)
target_link_libraries(test_jpl rocket)


