# Imports the prebuilt cgvis library (cgvis/lib) as the target 'cgvis';
# also imports cg, on which cgvis depends (cg must be a sibling of cgvis)
#
# Usage, after project():
#   include(<path to cgvis>/import.cmake)
#   target_link_libraries(<target> PRIVATE cgvis)

# Include only once
if(TARGET cgvis)
  return()
endif()

include(${CMAKE_CURRENT_LIST_DIR}/../cg/import.cmake)

set(CGVIS_DIR ${CMAKE_CURRENT_LIST_DIR})
set(CGVIS_LIB_PREFIX ${CGVIS_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX})

add_library(cgvis STATIC IMPORTED)
set_target_properties(cgvis PROPERTIES
  # cgvis.lib / libcgvis.a for Release (and any non-Debug configuration)
  IMPORTED_LOCATION ${CGVIS_LIB_PREFIX}cgvis${CMAKE_STATIC_LIBRARY_SUFFIX}
  # cgvisD.lib / libcgvisD.a for Debug
  IMPORTED_LOCATION_DEBUG ${CGVIS_LIB_PREFIX}cgvisD${CMAKE_STATIC_LIBRARY_SUFFIX}
  INTERFACE_INCLUDE_DIRECTORIES "${CGVIS_DIR}/include"
)

# cgvis uses cg: linking cgvis also links cg (after it) and brings its interface
target_link_libraries(cgvis INTERFACE cg)
