# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/fabien/.local/lib/python3.6/site-packages/cmake/data/bin/cmake

# The command to remove a file.
RM = /home/fabien/.local/lib/python3.6/site-packages/cmake/data/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/fabien/UV-AMSE/sysEmb/Github/AMSE_Projet_C

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/fabien/UV-AMSE/sysEmb/Github/AMSE_Projet_C/build

# Include any dependencies generated for this target.
include CMakeFiles/ecrimemparcycle.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/ecrimemparcycle.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/ecrimemparcycle.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ecrimemparcycle.dir/flags.make

CMakeFiles/ecrimemparcycle.dir/src/ecrimemparcycle.c.o: CMakeFiles/ecrimemparcycle.dir/flags.make
CMakeFiles/ecrimemparcycle.dir/src/ecrimemparcycle.c.o: ../src/ecrimemparcycle.c
CMakeFiles/ecrimemparcycle.dir/src/ecrimemparcycle.c.o: CMakeFiles/ecrimemparcycle.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fabien/UV-AMSE/sysEmb/Github/AMSE_Projet_C/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/ecrimemparcycle.dir/src/ecrimemparcycle.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/ecrimemparcycle.dir/src/ecrimemparcycle.c.o -MF CMakeFiles/ecrimemparcycle.dir/src/ecrimemparcycle.c.o.d -o CMakeFiles/ecrimemparcycle.dir/src/ecrimemparcycle.c.o -c /home/fabien/UV-AMSE/sysEmb/Github/AMSE_Projet_C/src/ecrimemparcycle.c

CMakeFiles/ecrimemparcycle.dir/src/ecrimemparcycle.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ecrimemparcycle.dir/src/ecrimemparcycle.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/fabien/UV-AMSE/sysEmb/Github/AMSE_Projet_C/src/ecrimemparcycle.c > CMakeFiles/ecrimemparcycle.dir/src/ecrimemparcycle.c.i

CMakeFiles/ecrimemparcycle.dir/src/ecrimemparcycle.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ecrimemparcycle.dir/src/ecrimemparcycle.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/fabien/UV-AMSE/sysEmb/Github/AMSE_Projet_C/src/ecrimemparcycle.c -o CMakeFiles/ecrimemparcycle.dir/src/ecrimemparcycle.c.s

# Object files for target ecrimemparcycle
ecrimemparcycle_OBJECTS = \
"CMakeFiles/ecrimemparcycle.dir/src/ecrimemparcycle.c.o"

# External object files for target ecrimemparcycle
ecrimemparcycle_EXTERNAL_OBJECTS =

../bin/ecrimemparcycle: CMakeFiles/ecrimemparcycle.dir/src/ecrimemparcycle.c.o
../bin/ecrimemparcycle: CMakeFiles/ecrimemparcycle.dir/build.make
../bin/ecrimemparcycle: CMakeFiles/ecrimemparcycle.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/fabien/UV-AMSE/sysEmb/Github/AMSE_Projet_C/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable ../bin/ecrimemparcycle"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ecrimemparcycle.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ecrimemparcycle.dir/build: ../bin/ecrimemparcycle
.PHONY : CMakeFiles/ecrimemparcycle.dir/build

CMakeFiles/ecrimemparcycle.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ecrimemparcycle.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ecrimemparcycle.dir/clean

CMakeFiles/ecrimemparcycle.dir/depend:
	cd /home/fabien/UV-AMSE/sysEmb/Github/AMSE_Projet_C/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/fabien/UV-AMSE/sysEmb/Github/AMSE_Projet_C /home/fabien/UV-AMSE/sysEmb/Github/AMSE_Projet_C /home/fabien/UV-AMSE/sysEmb/Github/AMSE_Projet_C/build /home/fabien/UV-AMSE/sysEmb/Github/AMSE_Projet_C/build /home/fabien/UV-AMSE/sysEmb/Github/AMSE_Projet_C/build/CMakeFiles/ecrimemparcycle.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ecrimemparcycle.dir/depend

