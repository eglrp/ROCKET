# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/kfkuang/rocket

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kfkuang/rocket

# Include any dependencies generated for this target.
include apps/network/CMakeFiles/clksmpc.dir/depend.make

# Include the progress variables for this target.
include apps/network/CMakeFiles/clksmpc.dir/progress.make

# Include the compile flags for this target's objects.
include apps/network/CMakeFiles/clksmpc.dir/flags.make

apps/network/CMakeFiles/clksmpc.dir/clksmpc.cpp.o: apps/network/CMakeFiles/clksmpc.dir/flags.make
apps/network/CMakeFiles/clksmpc.dir/clksmpc.cpp.o: apps/network/clksmpc.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/kfkuang/rocket/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object apps/network/CMakeFiles/clksmpc.dir/clksmpc.cpp.o"
	cd /home/kfkuang/rocket/apps/network && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/clksmpc.dir/clksmpc.cpp.o -c /home/kfkuang/rocket/apps/network/clksmpc.cpp

apps/network/CMakeFiles/clksmpc.dir/clksmpc.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/clksmpc.dir/clksmpc.cpp.i"
	cd /home/kfkuang/rocket/apps/network && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/kfkuang/rocket/apps/network/clksmpc.cpp > CMakeFiles/clksmpc.dir/clksmpc.cpp.i

apps/network/CMakeFiles/clksmpc.dir/clksmpc.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/clksmpc.dir/clksmpc.cpp.s"
	cd /home/kfkuang/rocket/apps/network && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/kfkuang/rocket/apps/network/clksmpc.cpp -o CMakeFiles/clksmpc.dir/clksmpc.cpp.s

apps/network/CMakeFiles/clksmpc.dir/clksmpc.cpp.o.requires:
.PHONY : apps/network/CMakeFiles/clksmpc.dir/clksmpc.cpp.o.requires

apps/network/CMakeFiles/clksmpc.dir/clksmpc.cpp.o.provides: apps/network/CMakeFiles/clksmpc.dir/clksmpc.cpp.o.requires
	$(MAKE) -f apps/network/CMakeFiles/clksmpc.dir/build.make apps/network/CMakeFiles/clksmpc.dir/clksmpc.cpp.o.provides.build
.PHONY : apps/network/CMakeFiles/clksmpc.dir/clksmpc.cpp.o.provides

apps/network/CMakeFiles/clksmpc.dir/clksmpc.cpp.o.provides.build: apps/network/CMakeFiles/clksmpc.dir/clksmpc.cpp.o

# Object files for target clksmpc
clksmpc_OBJECTS = \
"CMakeFiles/clksmpc.dir/clksmpc.cpp.o"

# External object files for target clksmpc
clksmpc_EXTERNAL_OBJECTS =

apps/network/clksmpc: apps/network/CMakeFiles/clksmpc.dir/clksmpc.cpp.o
apps/network/clksmpc: librocket.so
apps/network/clksmpc: apps/network/CMakeFiles/clksmpc.dir/build.make
apps/network/clksmpc: apps/network/CMakeFiles/clksmpc.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable clksmpc"
	cd /home/kfkuang/rocket/apps/network && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/clksmpc.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
apps/network/CMakeFiles/clksmpc.dir/build: apps/network/clksmpc
.PHONY : apps/network/CMakeFiles/clksmpc.dir/build

apps/network/CMakeFiles/clksmpc.dir/requires: apps/network/CMakeFiles/clksmpc.dir/clksmpc.cpp.o.requires
.PHONY : apps/network/CMakeFiles/clksmpc.dir/requires

apps/network/CMakeFiles/clksmpc.dir/clean:
	cd /home/kfkuang/rocket/apps/network && $(CMAKE_COMMAND) -P CMakeFiles/clksmpc.dir/cmake_clean.cmake
.PHONY : apps/network/CMakeFiles/clksmpc.dir/clean

apps/network/CMakeFiles/clksmpc.dir/depend:
	cd /home/kfkuang/rocket && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kfkuang/rocket /home/kfkuang/rocket/apps/network /home/kfkuang/rocket /home/kfkuang/rocket/apps/network /home/kfkuang/rocket/apps/network/CMakeFiles/clksmpc.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : apps/network/CMakeFiles/clksmpc.dir/depend
