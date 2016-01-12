# apps/dev/CMakeLists.txt

add_executable(upd upd.cpp)
target_link_libraries(upd gpstk)
install (TARGETS upd DESTINATION bin)

add_executable(updzap updzap.cpp)
target_link_libraries(updzap gpstk)
install (TARGETS updzap DESTINATION bin)

add_executable(clkupd clkupd.cpp)
target_link_libraries(clkupd gpstk)
install (TARGETS clkupd DESTINATION bin)

add_executable(clkupdzap clkupdzap.cpp)
target_link_libraries(clkupdzap gpstk)
install (TARGETS clkupdzap DESTINATION bin)

add_executable(clk clk.cpp)
target_link_libraries(clk gpstk)
install (TARGETS clk DESTINATION bin)

add_executable(clksmpc clksmpc.cpp)
target_link_libraries(clksmpc gpstk)
install (TARGETS clksmpc DESTINATION bin)
