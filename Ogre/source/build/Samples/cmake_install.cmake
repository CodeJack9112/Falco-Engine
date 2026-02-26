# Install script for directory: D:/Dev/C++/FalcoEngine/Ogre/source/Samples

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "D:/Dev/C++/FalcoEngine/Ogre/source/build/sdk")
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

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/OGRE" TYPE FILE FILES
    "D:/Dev/C++/FalcoEngine/Ogre/source/Samples/Common/include/DefaultSamplesPlugin.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Samples/Common/include/ListenerFactoryLogic.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Samples/Common/include/Sample.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Samples/Common/include/SampleContext.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Samples/Common/include/SamplePlugin.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Samples/Common/include/SdkSample.h"
    )
endif()

