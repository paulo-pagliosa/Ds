cmake_minimum_required(VERSION 3.16)

if(POLICY CMP0091)
  # Allow MSVC_RUNTIME_LIBRARY to control the CRT (/MD, /MDd)
  # Must be set before project()
  cmake_policy(SET CMP0091 NEW)
endif()

project(myapp CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(myapp
  Main.cpp
  MainWindow.cpp
)

target_link_libraries(myapp cg)

set(MYAPP_RUNTIME_OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR})
set_target_properties(myapp PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${MYAPP_RUNTIME_OUTPUT_DIR}
)
foreach(MYAPP_OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
  string(TOUPPER ${MYAPP_OUTPUTCONFIG} MYAPP_OUTPUTCONFIG_UPPER)
  set_target_properties(myapp PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_${MYAPP_OUTPUTCONFIG_UPPER} ${MYAPP_RUNTIME_OUTPUT_DIR}
  )
endforeach()

# Whole-program optimization in Release
include(CheckIPOSupported)
check_ipo_supported(RESULT MYAPP_IPO_SUPPORTED OUTPUT MYAPP_IPO_ERROR)
if(MYAPP_IPO_SUPPORTED)
  set_target_properties(myapp PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()

if(MSVC)
  target_compile_options(myapp PRIVATE /W3 /sdl /permissive-)
  # RuntimeLibrary MultiThreaded(Debug)DLL
  set_target_properties(myapp PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
  )
endif()
