# Install script for directory: D:/VscodiumZone/FortyEighthProj/flac-1.3.3

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/libFLAC")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "D:/llvm-mingw-20240606-ucrt-x86_64/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/VscodiumZone/FortyEighthProj/flac-1.3.3/bld/libFLAC.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/FLAC" TYPE FILE FILES
    "D:/VscodiumZone/FortyEighthProj/flac-1.3.3/include/FLAC/all.h"
    "D:/VscodiumZone/FortyEighthProj/flac-1.3.3/include/FLAC/assert.h"
    "D:/VscodiumZone/FortyEighthProj/flac-1.3.3/include/FLAC/callback.h"
    "D:/VscodiumZone/FortyEighthProj/flac-1.3.3/include/FLAC/export.h"
    "D:/VscodiumZone/FortyEighthProj/flac-1.3.3/include/FLAC/format.h"
    "D:/VscodiumZone/FortyEighthProj/flac-1.3.3/include/FLAC/metadata.h"
    "D:/VscodiumZone/FortyEighthProj/flac-1.3.3/include/FLAC/ordinals.h"
    "D:/VscodiumZone/FortyEighthProj/flac-1.3.3/include/FLAC/stream_decoder.h"
    "D:/VscodiumZone/FortyEighthProj/flac-1.3.3/include/FLAC/stream_encoder.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "D:/VscodiumZone/FortyEighthProj/flac-1.3.3/bld/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
