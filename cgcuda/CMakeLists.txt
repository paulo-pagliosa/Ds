cmake_minimum_required(VERSION 3.16)

if(POLICY CMP0091)
  # Allow MSVC_RUNTIME_LIBRARY to control the CRT (/MD, /MDd)
  # Must be set before project()
  cmake_policy(SET CMP0091 NEW)
endif()

project(cgcuda CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(cgcuda STATIC
  src/CUDAHelper.cpp
)

target_include_directories(cgcuda PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/include
)

# _USE_CUDA must be propagated
find_package(CUDA REQUIRED)
target_compile_definitions(cgcuda PUBLIC _USE_CUDA)

target_link_libraries(cgcuda cg CUDA::cudart_static)

set(CGCUDA_LIB_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set_target_properties(cgcuda PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${CGCUDA_LIB_OUTPUT_DIR}
  DEBUG_POSTFIX "D"
)
foreach(CGCUDA_OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
  string(TOUPPER ${CGCUDA_OUTPUTCONFIG} CGCUDA_OUTPUTCONFIG_UPPER)
  set_target_properties(cgcuda PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY_${CGCUDA_OUTPUTCONFIG_UPPER} ${CGCUDA_LIB_OUTPUT_DIR}
  )
endforeach()

include(CheckIPOSupported)
check_ipo_supported(RESULT CGCUDA_IPO_SUPPORTED OUTPUT CGCUDA_IPO_ERROR)
if(CGCUDA_IPO_SUPPORTED)
  set_target_properties(cgcuda PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()

if(MSVC)
  target_compile_options(cgcuda PRIVATE /W3)

  # RuntimeLibrary MultiThreaded(Debug)DLL
  set_target_properties(cgcuda PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
  )
endif()
