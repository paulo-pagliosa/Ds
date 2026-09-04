cmake_minimum_required(VERSION 3.16)

if(POLICY CMP0091)
  # Allow MSVC_RUNTIME_LIBRARY to control the CRT (/MD, /MDd)
  # Must be set before project()
  cmake_policy(SET CMP0091 NEW)
endif()

project(cgvis CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(cgvis STATIC
  src/BoxSource.cpp
  src/ConeSource.cpp
  src/CylinderSource.cpp
  src/Glyph3.cpp
  src/LookupTable.cpp
  src/Mapper.cpp
  src/PolyData.cpp
  src/PolyDataMapper.cpp
  src/PolyMesh.cpp
  src/PolyMeshBuilder.cpp
  src/PolyMeshMapper.cpp
  src/Scalars.cpp
  src/Scene.cpp
  src/SceneWindow.cpp
  src/Source.cpp
  src/SphereSource.cpp
  src/Transform.cpp
  src/TriCell.cpp
  src/TriCellMesh.cpp
  src/TriCellMeshMapper.cpp
  src/TriCellMeshNormals.cpp
  src/TriCellMeshReader.cpp
)

target_include_directories(cgvis PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/include/
)

target_link_libraries(cgvis cg)

set(CGVIS_LIB_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set_target_properties(cgvis PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${CGVIS_LIB_OUTPUT_DIR}
  DEBUG_POSTFIX "D"
)
foreach(CGVIS_OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
  string(TOUPPER ${CGVIS_OUTPUTCONFIG} CGVIS_OUTPUTCONFIG_UPPER)
  set_target_properties(cgvis PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY_${CGVIS_OUTPUTCONFIG_UPPER} ${CGVIS_LIB_OUTPUT_DIR}
  )
endforeach()

include(CheckIPOSupported)
check_ipo_supported(RESULT CGVIS_IPO_SUPPORTED OUTPUT CGVIS_IPO_ERROR)
if(CGVIS_IPO_SUPPORTED)
  set_target_properties(cgvis PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()

if(MSVC)
  target_compile_options(cgvis PRIVATE /W3 /sdl /permissive-)
  # RuntimeLibrary MultiThreaded(Debug)DLL, i.e. dynamic CRT (/MD, /MDd)
  set_target_properties(cgvis PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
  )
endif()
