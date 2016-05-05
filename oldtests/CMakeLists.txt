# oldtests/CMakeLists.txt

add_executable(test_egm test_egm.cpp)
target_link_libraries(test_egm rocket)

add_executable(test_srp test_srp.cpp)
target_link_libraries(test_srp rocket)

add_executable(test_sp3 test_sp3.cpp)
target_link_libraries(test_sp3 rocket)

add_executable(test_iers test_iers.cpp)
target_link_libraries(test_iers rocket)

add_executable(test_de test_de.cpp)
target_link_libraries(test_de rocket)

add_executable(test_eop test_eop.cpp)
target_link_libraries(test_eop rocket)
