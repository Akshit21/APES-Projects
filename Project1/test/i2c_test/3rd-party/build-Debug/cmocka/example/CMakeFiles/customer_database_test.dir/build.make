# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/cmocka

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/build-Debug/cmocka

# Include any dependencies generated for this target.
include example/CMakeFiles/customer_database_test.dir/depend.make

# Include the progress variables for this target.
include example/CMakeFiles/customer_database_test.dir/progress.make

# Include the compile flags for this target's objects.
include example/CMakeFiles/customer_database_test.dir/flags.make

example/CMakeFiles/customer_database_test.dir/customer_database.c.o: example/CMakeFiles/customer_database_test.dir/flags.make
example/CMakeFiles/customer_database_test.dir/customer_database.c.o: /home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/cmocka/example/customer_database.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/build-Debug/cmocka/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object example/CMakeFiles/customer_database_test.dir/customer_database.c.o"
	cd /home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/build-Debug/cmocka/example && /usr/bin/gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/customer_database_test.dir/customer_database.c.o   -c /home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/cmocka/example/customer_database.c

example/CMakeFiles/customer_database_test.dir/customer_database.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/customer_database_test.dir/customer_database.c.i"
	cd /home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/build-Debug/cmocka/example && /usr/bin/gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/cmocka/example/customer_database.c > CMakeFiles/customer_database_test.dir/customer_database.c.i

example/CMakeFiles/customer_database_test.dir/customer_database.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/customer_database_test.dir/customer_database.c.s"
	cd /home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/build-Debug/cmocka/example && /usr/bin/gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/cmocka/example/customer_database.c -o CMakeFiles/customer_database_test.dir/customer_database.c.s

example/CMakeFiles/customer_database_test.dir/customer_database.c.o.requires:

.PHONY : example/CMakeFiles/customer_database_test.dir/customer_database.c.o.requires

example/CMakeFiles/customer_database_test.dir/customer_database.c.o.provides: example/CMakeFiles/customer_database_test.dir/customer_database.c.o.requires
	$(MAKE) -f example/CMakeFiles/customer_database_test.dir/build.make example/CMakeFiles/customer_database_test.dir/customer_database.c.o.provides.build
.PHONY : example/CMakeFiles/customer_database_test.dir/customer_database.c.o.provides

example/CMakeFiles/customer_database_test.dir/customer_database.c.o.provides.build: example/CMakeFiles/customer_database_test.dir/customer_database.c.o


example/CMakeFiles/customer_database_test.dir/customer_database_test.c.o: example/CMakeFiles/customer_database_test.dir/flags.make
example/CMakeFiles/customer_database_test.dir/customer_database_test.c.o: /home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/cmocka/example/customer_database_test.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/build-Debug/cmocka/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object example/CMakeFiles/customer_database_test.dir/customer_database_test.c.o"
	cd /home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/build-Debug/cmocka/example && /usr/bin/gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/customer_database_test.dir/customer_database_test.c.o   -c /home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/cmocka/example/customer_database_test.c

example/CMakeFiles/customer_database_test.dir/customer_database_test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/customer_database_test.dir/customer_database_test.c.i"
	cd /home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/build-Debug/cmocka/example && /usr/bin/gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/cmocka/example/customer_database_test.c > CMakeFiles/customer_database_test.dir/customer_database_test.c.i

example/CMakeFiles/customer_database_test.dir/customer_database_test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/customer_database_test.dir/customer_database_test.c.s"
	cd /home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/build-Debug/cmocka/example && /usr/bin/gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/cmocka/example/customer_database_test.c -o CMakeFiles/customer_database_test.dir/customer_database_test.c.s

example/CMakeFiles/customer_database_test.dir/customer_database_test.c.o.requires:

.PHONY : example/CMakeFiles/customer_database_test.dir/customer_database_test.c.o.requires

example/CMakeFiles/customer_database_test.dir/customer_database_test.c.o.provides: example/CMakeFiles/customer_database_test.dir/customer_database_test.c.o.requires
	$(MAKE) -f example/CMakeFiles/customer_database_test.dir/build.make example/CMakeFiles/customer_database_test.dir/customer_database_test.c.o.provides.build
.PHONY : example/CMakeFiles/customer_database_test.dir/customer_database_test.c.o.provides

example/CMakeFiles/customer_database_test.dir/customer_database_test.c.o.provides.build: example/CMakeFiles/customer_database_test.dir/customer_database_test.c.o


# Object files for target customer_database_test
customer_database_test_OBJECTS = \
"CMakeFiles/customer_database_test.dir/customer_database.c.o" \
"CMakeFiles/customer_database_test.dir/customer_database_test.c.o"

# External object files for target customer_database_test
customer_database_test_EXTERNAL_OBJECTS =

example/customer_database_test: example/CMakeFiles/customer_database_test.dir/customer_database.c.o
example/customer_database_test: example/CMakeFiles/customer_database_test.dir/customer_database_test.c.o
example/customer_database_test: example/CMakeFiles/customer_database_test.dir/build.make
example/customer_database_test: src/libcmocka.so.0.4.0
example/customer_database_test: example/CMakeFiles/customer_database_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/build-Debug/cmocka/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable customer_database_test"
	cd /home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/build-Debug/cmocka/example && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/customer_database_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
example/CMakeFiles/customer_database_test.dir/build: example/customer_database_test

.PHONY : example/CMakeFiles/customer_database_test.dir/build

example/CMakeFiles/customer_database_test.dir/requires: example/CMakeFiles/customer_database_test.dir/customer_database.c.o.requires
example/CMakeFiles/customer_database_test.dir/requires: example/CMakeFiles/customer_database_test.dir/customer_database_test.c.o.requires

.PHONY : example/CMakeFiles/customer_database_test.dir/requires

example/CMakeFiles/customer_database_test.dir/clean:
	cd /home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/build-Debug/cmocka/example && $(CMAKE_COMMAND) -P CMakeFiles/customer_database_test.dir/cmake_clean.cmake
.PHONY : example/CMakeFiles/customer_database_test.dir/clean

example/CMakeFiles/customer_database_test.dir/depend:
	cd /home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/build-Debug/cmocka && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/cmocka /home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/cmocka/example /home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/build-Debug/cmocka /home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/build-Debug/cmocka/example /home/shuting/Desktop/APES-Projects/Project1/test/3rd-party/build-Debug/cmocka/example/CMakeFiles/customer_database_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : example/CMakeFiles/customer_database_test.dir/depend

