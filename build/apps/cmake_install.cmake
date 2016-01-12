# Install script for directory: /Users/shjzhang/Documents/Develop/gpstk/apps

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/Users/shjzhang/Documents/Develop/bin/gpstk")
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

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/checktools/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/clocktools/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/converters/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/DataAvailability/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/differential/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/difftools/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/filetools/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/geomatics/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/ionosphere/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/mergetools/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/MDPtools/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/rfw/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/swrx/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/receiver/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/multipath/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/performance/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/positioning/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/reszilla/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/Rinextools/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/time/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/visibility/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/dev/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/network/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/ephint/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/ssc/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/cc2noncc/cmake_install.cmake")
  INCLUDE("/Users/shjzhang/Documents/Develop/gpstk/build/apps/rnxfilter/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

