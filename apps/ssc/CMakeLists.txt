# apps/filetools/CMakeLists.txt

add_executable(ssc2msc ssc2msc.cpp)
target_link_libraries(ssc2msc gpstk)
install (TARGETS ssc2msc DESTINATION bin)

