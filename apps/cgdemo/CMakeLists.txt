cmake_minimum_required(VERSION 3.16)

if(POLICY CMP0091)
  # Allow MSVC_RUNTIME_LIBRARY to control the CRT (/MD, /MDd)
  # Must be set before project()
  cmake_policy(SET CMP0091 NEW)
endif()

project(cgdemo CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(cgdemo
  Main.cpp
  MainWindow.cpp
  RayTracer.cpp
  SceneWriter.cpp
  Writer.cpp
  reader/AbstractParser.cpp
  reader/Buffer.cpp
  reader/ErrorHandler.cpp
  reader/Expression.cpp
  reader/FileBuffer.cpp
  reader/ReaderBase.cpp
  reader/SceneReader.cpp
  reader/Scope.cpp
)

target_link_libraries(cgdemo cg)

set(CGDEMO_RUNTIME_OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR})
set_target_properties(cgdemo PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CGDEMO_RUNTIME_OUTPUT_DIR}
)
foreach(CGDEMO_OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
  string(TOUPPER ${CGDEMO_OUTPUTCONFIG} CGDEMO_OUTPUTCONFIG_UPPER)
  set_target_properties(cgdemo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_${CGDEMO_OUTPUTCONFIG_UPPER} ${CGDEMO_RUNTIME_OUTPUT_DIR}
  )
endforeach()

# Whole-program optimization in Release
include(CheckIPOSupported)
check_ipo_supported(RESULT CGDEMO_IPO_SUPPORTED OUTPUT CGDEMO_IPO_ERROR)
if(CGDEMO_IPO_SUPPORTED)
  set_target_properties(cgdemo PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()

if(MSVC)
  target_compile_options(cgdemo PRIVATE /W3 /sdl /permissive-)
  # RuntimeLibrary MultiThreaded(Debug)DLL
  set_target_properties(cgdemo PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
  )
endif()
