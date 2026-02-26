# Install script for directory: D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem

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
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/Debug/OgreRTShaderSystem_d.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/Release/OgreRTShaderSystem.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/MinSizeRel/OgreRTShaderSystem.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/RelWithDebInfo/OgreRTShaderSystem.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/Debug/OgreRTShaderSystem_d.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/Release/OgreRTShaderSystem.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/MinSizeRel/OgreRTShaderSystem.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/RelWithDebInfo/OgreRTShaderSystem.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/Debug/OgreRTShaderSystem_d.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/RelWithDebInfo/OgreRTShaderSystem.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/OGRE/RTShaderSystem" TYPE FILE FILES
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreRTShaderSystem.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderCGProgramProcessor.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderCGProgramWriter.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderExDualQuaternionSkinning.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderExHardwareSkinning.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderExHardwareSkinningTechnique.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderExIntegratedPSSM3.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderExLayeredBlending.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderExLinearSkinning.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderExNormalMapLighting.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderExPerPixelLighting.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderExTextureAtlasSampler.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderExTriplanarTexturing.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderFFPAlphaTest.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderFFPColour.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderFFPFog.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderFFPLighting.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderFFPRenderState.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderFFPRenderStateBuilder.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderFFPTexturing.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderFFPTransform.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderFunction.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderFunctionAtom.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderGLSLESProgramProcessor.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderGLSLESProgramWriter.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderGLSLProgramProcessor.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderGLSLProgramWriter.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderGenerator.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderHLSLProgramProcessor.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderHLSLProgramWriter.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderMaterialSerializerListener.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderParameter.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderPrerequisites.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderProgram.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderProgramManager.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderProgramProcessor.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderProgramSet.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderProgramWriter.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderProgramWriterManager.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderRenderState.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderScriptTranslator.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/Components/RTShaderSystem/include/OgreShaderSubRenderState.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/build/include/OgreRTShaderExports.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/build/include/OgreRTShaderConfig.h"
    )
endif()

