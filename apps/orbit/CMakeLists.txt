# apps/orbit/CMakeLists.txt

add_executable(orbitsim orbitsim.cpp)
target_link_libraries(orbitsim rocket)
install (TARGETS orbitsim DESTINATION bin)


