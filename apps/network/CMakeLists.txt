# apps/dev/CMakeLists.txt

#add_executable(upd upd.cpp)
#target_link_libraries(upd rocket)
#install (TARGETS upd DESTINATION bin)
#
#add_executable(updzap updzap.cpp)
#target_link_libraries(updzap rocket)
#install (TARGETS updzap DESTINATION bin)
#
#add_executable(clkupd clkupd.cpp)
#target_link_libraries(clkupd rocket)
#install (TARGETS clkupd DESTINATION bin)
#
#add_executable(clkupdzap clkupdzap.cpp)
#target_link_libraries(clkupdzap rocket)
#install (TARGETS clkupdzap DESTINATION bin)

#add_executable(clk clk.cpp)
#target_link_libraries(clk rocket)
#install (TARGETS clk DESTINATION bin)

add_executable(clk_rt clk_rt.cpp)
target_link_libraries(clk_rt rocket)
install (TARGETS clk_rt DESTINATION bin)
#add_executable(clksmpc clksmpc.cpp)
#target_link_libraries(clksmpc rocket)
#install (TARGETS clksmpc DESTINATION bin)
