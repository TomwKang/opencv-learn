# CMAKE generated file: DO NOT EDIT!
# Generated by "NMake Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF
SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "E:\JB2019\CLion 2020.3.1\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "E:\JB2019\CLion 2020.3.1\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\Project\RoboMaster\Vision\Module\cppcom

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\Project\RoboMaster\Vision\Module\cppcom\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles\MSB64Encoder.dir\depend.make

# Include the progress variables for this target.
include CMakeFiles\MSB64Encoder.dir\progress.make

# Include the compile flags for this target's objects.
include CMakeFiles\MSB64Encoder.dir\flags.make

CMakeFiles\MSB64Encoder.dir\src\MSB64Encoder.c.obj: CMakeFiles\MSB64Encoder.dir\flags.make
CMakeFiles\MSB64Encoder.dir\src\MSB64Encoder.c.obj: ..\src\MSB64Encoder.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\Project\RoboMaster\Vision\Module\cppcom\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/MSB64Encoder.dir/src/MSB64Encoder.c.obj"
	E:\VS2019\Programs\VC\Tools\MSVC\14.28.29333\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\MSB64Encoder.dir\src\MSB64Encoder.c.obj /FdCMakeFiles\MSB64Encoder.dir\MSB64Encoder.pdb /FS -c D:\Project\RoboMaster\Vision\Module\cppcom\src\MSB64Encoder.c
<<

CMakeFiles\MSB64Encoder.dir\src\MSB64Encoder.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/MSB64Encoder.dir/src/MSB64Encoder.c.i"
	E:\VS2019\Programs\VC\Tools\MSVC\14.28.29333\bin\Hostx86\x86\cl.exe > CMakeFiles\MSB64Encoder.dir\src\MSB64Encoder.c.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\Project\RoboMaster\Vision\Module\cppcom\src\MSB64Encoder.c
<<

CMakeFiles\MSB64Encoder.dir\src\MSB64Encoder.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/MSB64Encoder.dir/src/MSB64Encoder.c.s"
	E:\VS2019\Programs\VC\Tools\MSVC\14.28.29333\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\MSB64Encoder.dir\src\MSB64Encoder.c.s /c D:\Project\RoboMaster\Vision\Module\cppcom\src\MSB64Encoder.c
<<

# Object files for target MSB64Encoder
MSB64Encoder_OBJECTS = \
"CMakeFiles\MSB64Encoder.dir\src\MSB64Encoder.c.obj"

# External object files for target MSB64Encoder
MSB64Encoder_EXTERNAL_OBJECTS =

MSB64Encoder.lib: CMakeFiles\MSB64Encoder.dir\src\MSB64Encoder.c.obj
MSB64Encoder.lib: CMakeFiles\MSB64Encoder.dir\build.make
MSB64Encoder.lib: CMakeFiles\MSB64Encoder.dir\objects1.rsp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\Project\RoboMaster\Vision\Module\cppcom\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library MSB64Encoder.lib"
	$(CMAKE_COMMAND) -P CMakeFiles\MSB64Encoder.dir\cmake_clean_target.cmake
	E:\VS2019\Programs\VC\Tools\MSVC\14.28.29333\bin\Hostx86\x86\link.exe /lib /nologo /machine:X86 /out:MSB64Encoder.lib @CMakeFiles\MSB64Encoder.dir\objects1.rsp 

# Rule to build all files generated by this target.
CMakeFiles\MSB64Encoder.dir\build: MSB64Encoder.lib

.PHONY : CMakeFiles\MSB64Encoder.dir\build

CMakeFiles\MSB64Encoder.dir\clean:
	$(CMAKE_COMMAND) -P CMakeFiles\MSB64Encoder.dir\cmake_clean.cmake
.PHONY : CMakeFiles\MSB64Encoder.dir\clean

CMakeFiles\MSB64Encoder.dir\depend:
	$(CMAKE_COMMAND) -E cmake_depends "NMake Makefiles" D:\Project\RoboMaster\Vision\Module\cppcom D:\Project\RoboMaster\Vision\Module\cppcom D:\Project\RoboMaster\Vision\Module\cppcom\cmake-build-debug D:\Project\RoboMaster\Vision\Module\cppcom\cmake-build-debug D:\Project\RoboMaster\Vision\Module\cppcom\cmake-build-debug\CMakeFiles\MSB64Encoder.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles\MSB64Encoder.dir\depend

