# Imports the prebuilt cg library (cg/lib) as the target 'cg', together with
# its includes, definitions and dependencies
#
# Usage, after project():
#   include(<path to cg>/import.cmake)
#   target_link_libraries(<target> PRIVATE cg)

# Include only once, even if several imported libraries depend on cg
if(TARGET cg)
  return()
endif()

set(CG_DIR ${CMAKE_CURRENT_LIST_DIR})
set(CG_LIB_PREFIX ${CG_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX})

option(USE_CUDA "Enable CUDA support (cg must be built with it too)" OFF)

add_library(cg STATIC IMPORTED)
set_target_properties(cg PROPERTIES
  # cg.lib / libcg.a for Release (and any non-Debug configuration)
  IMPORTED_LOCATION ${CG_LIB_PREFIX}cg${CMAKE_STATIC_LIBRARY_SUFFIX}
  # cgD.lib / libcgD.a for Debug
  IMPORTED_LOCATION_DEBUG ${CG_LIB_PREFIX}cgD${CMAKE_STATIC_LIBRARY_SUFFIX}
  INTERFACE_INCLUDE_DIRECTORIES "${CG_DIR}/include;${CG_DIR}/externals/include"
)

# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(cg INTERFACE OpenGL::GL)

# GLFW
if(WIN32 OR APPLE)
  # Static library shipped in cg/externals/lib
  # (glfw3.lib on Windows, universal libglfw3.a on macOS)
  find_library(glfw3_LIB glfw3
    PATHS ${CG_DIR}/externals/lib
    NO_DEFAULT_PATH REQUIRED)
  target_link_libraries(cg INTERFACE ${glfw3_LIB})
endif()

if(APPLE)
  target_link_libraries(cg INTERFACE
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
    "-framework QuartzCore"
  )
  # macOS deprecates OpenGL; silence the warnings
  target_compile_definitions(cg INTERFACE GL_SILENCE_DEPRECATION)
elseif(NOT WIN32)
  # Linux: system package (e.g. libglfw3-dev)
  find_package(glfw3 REQUIRED)
  target_link_libraries(cg INTERFACE glfw)
endif()

# CUDA (not available on macOS)
if(USE_CUDA)
  if(APPLE)
    message(FATAL_ERROR "CUDA is not supported on macOS")
  endif()
  find_package(CUDAToolkit REQUIRED)
  target_compile_definitions(cg INTERFACE _USE_CUDA)
  target_link_libraries(cg INTERFACE CUDA::cudart CUDA::cuda_driver)
endif()
