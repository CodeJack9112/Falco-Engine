# Install script for directory: D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL

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
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/OGRE" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/Debug/RenderSystem_GL_d.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/OGRE" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/Release/RenderSystem_GL.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/OGRE" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/MinSizeRel/RenderSystem_GL.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/OGRE" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/lib/RelWithDebInfo/RenderSystem_GL.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/Debug/RenderSystem_GL_d.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/Release/RenderSystem_GL.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/MinSizeRel/RenderSystem_GL.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/RelWithDebInfo/RenderSystem_GL.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/Debug/RenderSystem_GL_d.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Dev/C++/FalcoEngine/Ogre/source/build/bin/RelWithDebInfo/RenderSystem_GL.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/OGRE/RenderSystems/GL" TYPE FILE FILES
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/include/OgreGLCopyingRenderTexture.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/include/OgreGLDepthBuffer.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/include/OgreGLFBOMultiRenderTarget.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/include/OgreGLFBORenderTexture.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/include/OgreGLFrameBufferObject.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/include/OgreGLGpuNvparseProgram.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/include/OgreGLGpuProgram.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/include/OgreGLGpuProgramManager.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/include/OgreGLHardwareBufferManager.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/include/OgreGLHardwareIndexBuffer.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/include/OgreGLHardwareOcclusionQuery.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/include/OgreGLHardwarePixelBuffer.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/include/OgreGLHardwareVertexBuffer.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/include/OgreGLPBRenderTexture.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/include/OgreGLPixelFormat.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/include/OgreGLPlugin.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/include/OgreGLPrerequisites.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/include/OgreGLRenderSystem.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/include/OgreGLRenderToVertexBuffer.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/include/OgreGLStateCacheManager.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/include/OgreGLTexture.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/include/OgreGLTextureManager.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/src/GLSL/include/OgreGLSLExtSupport.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/src/GLSL/include/OgreGLSLGpuProgram.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/src/GLSL/include/OgreGLSLLinkProgram.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/src/GLSL/include/OgreGLSLLinkProgramManager.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/src/GLSL/include/OgreGLSLProgram.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/src/GLSL/include/OgreGLSLProgramFactory.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/src/nvparse/_ps1.0_parser.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/src/nvparse/_rc1.0_parser.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/src/nvparse/_ts1.0_parser.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/src/nvparse/_vs1.0_parser.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/src/nvparse/macro.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/src/nvparse/nvparse.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/src/nvparse/nvparse_errors.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/src/nvparse/nvparse_externs.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/src/nvparse/ps1.0_program.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/src/nvparse/rc1.0_combiners.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/src/nvparse/rc1.0_final.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/src/nvparse/rc1.0_general.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/src/nvparse/rc1.0_register.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/src/nvparse/ts1.0_inst.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/src/nvparse/ts1.0_inst_list.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/src/nvparse/vs1.0_inst.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/src/nvparse/vs1.0_inst_list.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GLSupport/include/OgreGLContext.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GLSupport/include/OgreGLHardwarePixelBufferCommon.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GLSupport/include/OgreGLNativeSupport.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GLSupport/include/OgreGLPBuffer.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GLSupport/include/OgreGLRenderSystemCommon.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GLSupport/include/OgreGLRenderTexture.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GLSupport/include/OgreGLStateCacheManagerCommon.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GLSupport/include/OgreGLTextureCommon.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GLSupport/include/OgreGLUniformCache.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GLSupport/include/OgreGLUtil.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GLSupport/include/OgreGLVertexArrayObject.h"
    "D:/Dev/C++/FalcoEngine/Ogre/source/build/include/OgreGLSupportPrerequisites.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/OGRE/RenderSystems/GL" TYPE DIRECTORY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/include/GL")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/OGRE/RenderSystems/GL" TYPE DIRECTORY FILES "D:/Dev/C++/FalcoEngine/Ogre/source/RenderSystems/GL/src/GLSL/include/")
endif()

