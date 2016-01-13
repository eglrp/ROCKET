# Install script for directory: /Users/shjzhang/Documents/Develop/rocket/ROCKET/apps/network

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/Users/shjzhang/Documents/Develop/bin/rocket")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/Users/shjzhang/Documents/Develop/rocket/ROCKET/build/apps/network/upd")
  IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/upd" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/upd")
    EXECUTE_PROCESS(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/shjzhang/Documents/Develop/rocket/ROCKET/build/librocket.dylib" "/Users/shjzhang/Documents/Develop/bin/rocket/lib/librocket.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/upd")
    execute_process(COMMAND /usr/bin/install_name_tool
      -add_rpath "/Users/shjzhang/Documents/Develop/bin/rocket:$ORIGIN/../lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/upd")
  ENDIF()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/Users/shjzhang/Documents/Develop/rocket/ROCKET/build/apps/network/updzap")
  IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/updzap" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/updzap")
    EXECUTE_PROCESS(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/shjzhang/Documents/Develop/rocket/ROCKET/build/librocket.dylib" "/Users/shjzhang/Documents/Develop/bin/rocket/lib/librocket.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/updzap")
    execute_process(COMMAND /usr/bin/install_name_tool
      -add_rpath "/Users/shjzhang/Documents/Develop/bin/rocket:$ORIGIN/../lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/updzap")
  ENDIF()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/Users/shjzhang/Documents/Develop/rocket/ROCKET/build/apps/network/clkupd")
  IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/clkupd" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/clkupd")
    EXECUTE_PROCESS(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/shjzhang/Documents/Develop/rocket/ROCKET/build/librocket.dylib" "/Users/shjzhang/Documents/Develop/bin/rocket/lib/librocket.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/clkupd")
    execute_process(COMMAND /usr/bin/install_name_tool
      -add_rpath "/Users/shjzhang/Documents/Develop/bin/rocket:$ORIGIN/../lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/clkupd")
  ENDIF()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/Users/shjzhang/Documents/Develop/rocket/ROCKET/build/apps/network/clkupdzap")
  IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/clkupdzap" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/clkupdzap")
    EXECUTE_PROCESS(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/shjzhang/Documents/Develop/rocket/ROCKET/build/librocket.dylib" "/Users/shjzhang/Documents/Develop/bin/rocket/lib/librocket.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/clkupdzap")
    execute_process(COMMAND /usr/bin/install_name_tool
      -add_rpath "/Users/shjzhang/Documents/Develop/bin/rocket:$ORIGIN/../lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/clkupdzap")
  ENDIF()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/Users/shjzhang/Documents/Develop/rocket/ROCKET/build/apps/network/clk")
  IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/clk" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/clk")
    EXECUTE_PROCESS(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/shjzhang/Documents/Develop/rocket/ROCKET/build/librocket.dylib" "/Users/shjzhang/Documents/Develop/bin/rocket/lib/librocket.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/clk")
    execute_process(COMMAND /usr/bin/install_name_tool
      -add_rpath "/Users/shjzhang/Documents/Develop/bin/rocket:$ORIGIN/../lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/clk")
  ENDIF()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/Users/shjzhang/Documents/Develop/rocket/ROCKET/build/apps/network/clksmpc")
  IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/clksmpc" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/clksmpc")
    EXECUTE_PROCESS(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/shjzhang/Documents/Develop/rocket/ROCKET/build/librocket.dylib" "/Users/shjzhang/Documents/Develop/bin/rocket/lib/librocket.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/clksmpc")
    execute_process(COMMAND /usr/bin/install_name_tool
      -add_rpath "/Users/shjzhang/Documents/Develop/bin/rocket:$ORIGIN/../lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/clksmpc")
  ENDIF()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

