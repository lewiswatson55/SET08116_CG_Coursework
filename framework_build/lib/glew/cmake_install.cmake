# Install script for directory: C:/Users/lewis/OneDrive - Edinburgh Napier University/Modules/Second Year/2 CG/set08116/labs/framework/lib/glew

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/set08116_coursework")
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
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/lewis/OneDrive - Edinburgh Napier University/Modules/Second Year/2 CG/SET08116_CG_Coursework/bin/lib/Debug/libglew_sharedd.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/lewis/OneDrive - Edinburgh Napier University/Modules/Second Year/2 CG/SET08116_CG_Coursework/bin/lib/Release/libglew_shared.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/lewis/OneDrive - Edinburgh Napier University/Modules/Second Year/2 CG/SET08116_CG_Coursework/bin/lib/MinSizeRel/libglew_shared.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/lewis/OneDrive - Edinburgh Napier University/Modules/Second Year/2 CG/SET08116_CG_Coursework/bin/lib/RelWithDebInfo/libglew_shared.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "C:/Users/lewis/OneDrive - Edinburgh Napier University/Modules/Second Year/2 CG/SET08116_CG_Coursework/bin/bin/Debug/glewd.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "C:/Users/lewis/OneDrive - Edinburgh Napier University/Modules/Second Year/2 CG/SET08116_CG_Coursework/bin/bin/Release/glew.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "C:/Users/lewis/OneDrive - Edinburgh Napier University/Modules/Second Year/2 CG/SET08116_CG_Coursework/bin/bin/MinSizeRel/glew.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "C:/Users/lewis/OneDrive - Edinburgh Napier University/Modules/Second Year/2 CG/SET08116_CG_Coursework/bin/bin/RelWithDebInfo/glew.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/GL" TYPE FILE FILES
    "C:/Users/lewis/OneDrive - Edinburgh Napier University/Modules/Second Year/2 CG/set08116/labs/framework/lib/glew/include/GL/eglew.h"
    "C:/Users/lewis/OneDrive - Edinburgh Napier University/Modules/Second Year/2 CG/set08116/labs/framework/lib/glew/include/GL/glew.h"
    "C:/Users/lewis/OneDrive - Edinburgh Napier University/Modules/Second Year/2 CG/set08116/labs/framework/lib/glew/include/GL/glxew.h"
    "C:/Users/lewis/OneDrive - Edinburgh Napier University/Modules/Second Year/2 CG/set08116/labs/framework/lib/glew/include/GL/wglew.h"
    )
endif()

