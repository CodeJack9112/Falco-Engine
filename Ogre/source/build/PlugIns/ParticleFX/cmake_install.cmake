# Install script for directory: D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX

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
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/OGRE" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/Debug/Plugin_ParticleFX_d.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/OGRE" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/Release/Plugin_ParticleFX.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/OGRE" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/MinSizeRel/Plugin_ParticleFX.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/OGRE" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/RelWithDebInfo/Plugin_ParticleFX.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/Debug/Plugin_ParticleFX_d.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/Release/Plugin_ParticleFX.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/MinSizeRel/Plugin_ParticleFX.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/RelWithDebInfo/Plugin_ParticleFX.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/Debug/Plugin_ParticleFX_d.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/RelWithDebInfo/Plugin_ParticleFX.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/OGRE/Plugins/ParticleFX" TYPE FILE FILES
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreAreaEmitter.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreBoxEmitter.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreBoxEmitterFactory.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreColourFaderAffector.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreColourFaderAffector2.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreColourFaderAffectorFactory.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreColourFaderAffectorFactory2.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreColourImageAffector.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreColourImageAffectorFactory.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreColourInterpolatorAffector.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreColourInterpolatorAffectorFactory.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreCylinderEmitter.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreCylinderEmitterFactory.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreDeflectorPlaneAffector.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreDeflectorPlaneAffectorFactory.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreDirectionRandomiserAffector.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreDirectionRandomiserAffectorFactory.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreEllipsoidEmitter.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreEllipsoidEmitterFactory.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreHollowEllipsoidEmitter.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreHollowEllipsoidEmitterFactory.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreLinearForceAffector.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreLinearForceAffectorFactory.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreParticleFXPlugin.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgrePointEmitter.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgrePointEmitterFactory.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreRingEmitter.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreRingEmitterFactory.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreRotationAffector.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreRotationAffectorFactory.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreScaleAffector.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/PlugIns/ParticleFX/include/OgreScaleAffectorFactory.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/build/include/OgreParticleFXPrerequisites.h"
    )
endif()

