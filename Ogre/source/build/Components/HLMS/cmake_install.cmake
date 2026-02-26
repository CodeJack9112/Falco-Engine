# Install script for directory: D:/Dev/C++/FalcoEngine/Ogre/source/Components/HLMS

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
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/Debug/OgreHLMS_d.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/Release/OgreHLMS.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/MinSizeRel/OgreHLMS.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/RelWithDebInfo/OgreHLMS.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/Debug/OgreHLMS_d.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/Release/OgreHLMS.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/MinSizeRel/OgreHLMS.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/RelWithDebInfo/OgreHLMS.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/Debug/OgreHLMS_d.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/RelWithDebInfo/OgreHLMS.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/OGRE/HLMS" TYPE FILE FILES
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/HLMS/include/OgreHlmsDatablock.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/HLMS/include/OgreHlmsManager.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/HLMS/include/OgreHlmsMaterialBase.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/HLMS/include/OgreHlmsPbsMaterial.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/HLMS/include/OgreHlmsPropertyHelper.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/HLMS/include/OgreHlmsPropertyMap.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/HLMS/include/OgreHlmsShaderCommon.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/HLMS/include/OgreHlmsShaderGenerator.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/HLMS/include/OgreHlmsShaderManager.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/HLMS/include/OgreHlmsShaderPiecesManager.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/HLMS/include/OgreHlmsShaderTemplate.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/HLMS/include/OgreId.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/HLMS/include/OgreIdString.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/build/include/OgreHlmsPrerequisites.h"
    )
endif()

