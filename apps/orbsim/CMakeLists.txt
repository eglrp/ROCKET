# apps/orbsim/CMakeLists.txt

add_executable(orbsim orbsim.cpp)
target_link_libraries(orbsim rocket)
install (TARGETS orbsim DESTINATION bin)


