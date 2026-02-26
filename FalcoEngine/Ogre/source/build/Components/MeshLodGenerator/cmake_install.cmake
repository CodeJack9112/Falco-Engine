# Install script for directory: D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator

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
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/Debug/OgreMeshLodGenerator_d.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/Release/OgreMeshLodGenerator.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/MinSizeRel/OgreMeshLodGenerator.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/RelWithDebInfo/OgreMeshLodGenerator.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/Debug/OgreMeshLodGenerator_d.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/Release/OgreMeshLodGenerator.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/MinSizeRel/OgreMeshLodGenerator.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/RelWithDebInfo/OgreMeshLodGenerator.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/Debug/OgreMeshLodGenerator_d.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/RelWithDebInfo/OgreMeshLodGenerator.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/OGRE/MeshLodGenerator" TYPE FILE FILES
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreLod0Stripifier.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreLodBuffer.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreLodCollapseCost.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreLodCollapseCostCurvature.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreLodCollapseCostOutside.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreLodCollapseCostProfiler.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreLodCollapseCostQuadric.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreLodCollapser.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreLodConfig.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreLodConfigSerializer.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreLodData.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreLodInputProvider.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreLodInputProviderBuffer.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreLodInputProviderMesh.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreLodOutputProvider.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreLodOutputProviderBuffer.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreLodOutputProviderCompressedBuffer.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreLodOutputProviderCompressedMesh.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreLodOutputProviderMesh.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreLodOutsideMarker.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreLodPrerequisites.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreLodWorkQueueInjector.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreLodWorkQueueInjectorListener.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreLodWorkQueueRequest.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreLodWorkQueueWorker.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreMeshLodGenerator.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreSmallVector.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreVectorSet.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/MeshLodGenerator/include/OgreVectorSetImpl.h"
    )
endif()

