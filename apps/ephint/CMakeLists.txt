# apps/performance/CMakeLists.txt

add_executable(ephint ephint.cpp ComputeSatPCOffset.cpp )
target_link_libraries(ephint gpstk)
install (TARGETS ephint DESTINATION bin)


