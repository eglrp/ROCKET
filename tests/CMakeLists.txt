# tests/CMakeLists.txt

# EOP
add_executable(test_eop test_eop.cpp)
target_link_libraries(test_eop rocket)

# ORB
add_executable(test_orbit_fit test_orbit_fit.cpp)
target_link_libraries(test_orbit_fit rocket)

add_executable(test_oi test_oi.cpp)
target_link_libraries(test_oi rocket)

add_executable(test_orbit_cmp test_orbit_cmp.cpp)
target_link_libraries(test_orbit_cmp rocket)

# FILE
add_executable(rinex_obs_test rinex_obs_test.cpp)
target_link_libraries(rinex_obs_test rocket)

add_executable(rinex_nav_test rinex_nav_test.cpp)
target_link_libraries(rinex_nav_test rocket)

add_executable(rinex_clk_test rinex_clk_test.cpp)
target_link_libraries(rinex_clk_test rocket)

# TropModel
add_executable(test_tropmodel test_tropmodel.cpp)
target_link_libraries(test_tropmodel rocket)

# ORB and CLK
add_executable(test_stapos test_stapos.cpp)
target_link_libraries(test_stapos rocket)

add_executable(gps_clock1 gps_clock1.cpp)
target_link_libraries(gps_clock1 rocket)

add_executable(gps_clock3 gps_clock3.cpp)
target_link_libraries(gps_clock3 rocket)

add_executable(test_phi test_phi.cpp)
target_link_libraries(test_phi rocket)

add_executable(gps_orbit1 gps_orbit1.cpp)
target_link_libraries(gps_orbit1 rocket)

add_executable(gps_orbclk1 gps_orbclk1.cpp)
target_link_libraries(gps_orbclk1 rocket)

add_executable(gps_orbclk3 gps_orbclk3.cpp)
target_link_libraries(gps_orbclk3 rocket)

add_executable(test_bce test_bce.cpp)
target_link_libraries(test_bce rocket)

add_executable(ssc2msc ssc2msc.cpp)
target_link_libraries(ssc2msc rocket)

add_executable(asc2bin asc2bin.cpp)
target_link_libraries(asc2bin rocket)

add_executable(signal signal.cpp)
target_link_libraries(signal rocket)

add_executable(station station.cpp)
target_link_libraries(station rocket)

add_executable(sp3_compare sp3_compare.cpp)
target_link_libraries(sp3_compare rocket)

add_executable(bce_sp3_compare bce_sp3_compare.cpp)
target_link_libraries(bce_sp3_compare rocket)

