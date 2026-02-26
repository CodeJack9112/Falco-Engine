# Install script for directory: D:/Dev/C++/FalcoEngine/Ogre/source/Components/Overlay

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
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/Debug/OgreOverlay_d.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/Release/OgreOverlay.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/MinSizeRel/OgreOverlay.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/RelWithDebInfo/OgreOverlay.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/Debug/OgreOverlay_d.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/Release/OgreOverlay.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/MinSizeRel/OgreOverlay.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/RelWithDebInfo/OgreOverlay.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/Debug/OgreOverlay_d.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/RelWithDebInfo/OgreOverlay.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/OGRE/Overlay" TYPE FILE FILES
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Overlay/include/OgreBorderPanelOverlayElement.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Overlay/include/OgreFont.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Overlay/include/OgreFontManager.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Overlay/include/OgreOverlay.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Overlay/include/OgreOverlayContainer.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Overlay/include/OgreOverlayElement.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Overlay/include/OgreOverlayElementCommands.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Overlay/include/OgreOverlayElementFactory.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Overlay/include/OgreOverlayManager.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Overlay/include/OgreOverlayProfileSessionListener.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Overlay/include/OgreOverlaySystem.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Overlay/include/OgrePanelOverlayElement.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Overlay/include/OgreTextAreaOverlayElement.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/build/include/OgreOverlayPrerequisites.h"
    )
endif()

