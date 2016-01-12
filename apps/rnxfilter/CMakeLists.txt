# apps/performance/CMakeLists.txt

add_executable(rnxfilter rnxfilter.cpp)
target_link_libraries(rnxfilter gpstk)
install (TARGETS rnxfilter DESTINATION bin)


