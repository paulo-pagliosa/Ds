cmake_minimum_required(VERSION 3.16)

if(POLICY CMP0091)
  # Allow MSVC_RUNTIME_LIBRARY to control the CRT (/MD, /MDd)
  # Must be set before project()
  cmake_policy(SET CMP0091 NEW)
endif()

project(cgvisdemo CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(cgvisdemo
  Demo.cpp
  Main.cpp
  MainWindow.cpp
)

target_link_directories(cgvisdemo PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../../cg/lib
  ${CMAKE_CURRENT_LIST_DIR}/../../cgvis/lib
)
target_link_libraries(cgvisdemo
  $<$<CONFIG:Debug>:cgvisD>$<$<NOT:$<CONFIG:Debug>>:cgvis>
  $<$<CONFIG:Debug>:cgD>$<$<NOT:$<CONFIG:Debug>>:cg>
)

find_package(OpenGL REQUIRED)
target_link_libraries(cgvisdemo OpenGL::GL)

if(NOT WIN32)
  find_package(glfw3 REQUIRED)
  target_link_libraries(cgvisdemo glfw)
endif()

set(CGVISDEMO_RUNTIME_OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR})
set_target_properties(cgvisdemo PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CGVISDEMO_RUNTIME_OUTPUT_DIR}
)
foreach(CGVISDEMO_OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
  string(TOUPPER ${CGVISDEMO_OUTPUTCONFIG} CGVISDEMO_OUTPUTCONFIG_UPPER)
  set_target_properties(cgvisdemo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_${CGVISDEMO_OUTPUTCONFIG_UPPER} ${CGVISDEMO_RUNTIME_OUTPUT_DIR}
  )
endforeach()

# Whole-program optimization in Release
include(CheckIPOSupported)
check_ipo_supported(RESULT CGVISDEMO_IPO_SUPPORTED OUTPUT CGVISDEMO_IPO_ERROR)
if(CGVISDEMO_IPO_SUPPORTED)
  set_target_properties(cgvisdemo PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()

if(MSVC)
  target_compile_options(cgvisdemo PRIVATE /W3 /sdl /permissive-)
  # RuntimeLibrary MultiThreaded(Debug)DLL
  set_target_properties(cgvisdemo PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
  )
endif()