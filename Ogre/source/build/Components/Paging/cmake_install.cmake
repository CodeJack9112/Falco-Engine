# Install script for directory: D:/Dev/C++/FalcoEngine/Ogre/source/Components/Paging

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
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/Debug/OgrePaging_d.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/Release/OgrePaging.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/MinSizeRel/OgrePaging.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/RelWithDebInfo/OgrePaging.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/Debug/OgrePaging_d.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/Release/OgrePaging.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/MinSizeRel/OgrePaging.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/RelWithDebInfo/OgrePaging.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/Debug/OgrePaging_d.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/RelWithDebInfo/OgrePaging.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/OGRE/Paging" TYPE FILE FILES
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Paging/include/OgreGrid2DPageStrategy.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Paging/include/OgreGrid3DPageStrategy.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Paging/include/OgrePage.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Paging/include/OgrePageConnection.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Paging/include/OgrePageContent.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Paging/include/OgrePageContentCollection.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Paging/include/OgrePageContentCollectionFactory.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Paging/include/OgrePageContentFactory.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Paging/include/OgrePageFileFormats.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Paging/include/OgrePageManager.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Paging/include/OgrePageStrategy.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Paging/include/OgrePagedWorld.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Paging/include/OgrePagedWorldSection.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Paging/include/OgrePaging.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Paging/include/OgrePagingPrerequisites.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/Paging/include/OgreSimplePageContentCollection.h"
    )
endif()

