cmake_minimum_required(VERSION 3.16)

if(POLICY CMP0091)
  # Allow MSVC_RUNTIME_LIBRARY to control the CRT (/MD, /MDd)
  # Must be set before project()
  cmake_policy(SET CMP0091 NEW)
endif()

project(cg CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(POLICY CMP0072)
  # Use vendor neutral OpenGL library
  cmake_policy(SET CMP0072 NEW)
endif()

add_library(cg STATIC
  externals/src/gl3w.c
  externals/src/imgui_draw.cpp
  externals/src/imgui_impl_glfw.cpp
  externals/src/imgui_impl_opengl3.cpp
  externals/src/imgui_tables.cpp
  externals/src/imgui_widgets.cpp
  externals/src/imgui.cpp
  src/core/BlockAllocator.cpp
  src/core/Exception.cpp
  src/core/NamedObject.cpp
  src/debug/AnimatedAlgorithm.cpp
  src/geometry/BVH.cpp
  src/geometry/MeshSweeper.cpp
  src/geometry/TriangleMesh.cpp
  src/geometry/TriangleMeshBVH.cpp
  src/graph/CameraProxy.cpp
  src/graph/Component.cpp
  src/graph/LightProxy.cpp
  src/graph/PrimitiveProxy.cpp
  src/graph/SceneObject.cpp
  src/graph/SceneObjectBuilder.cpp
  src/graph/SceneWindow.cpp
  src/graph/Transform.cpp
  src/graphics/Application.cpp
  src/graphics/Assets.cpp
  src/graphics/AssetFolder.cpp
  src/graphics/Camera.cpp
  src/graphics/Color.cpp
  src/graphics/GLFramebuffer.cpp
  src/graphics/GLGraphics2.cpp
  src/graphics/GLGraphics3.cpp
  src/graphics/GLGraphicsBase.cpp
  src/graphics/GLImage.cpp
  src/graphics/GLLines3.cpp
  src/graphics/GLLines3Renderer.cpp
  src/graphics/GLMesh.cpp
  src/graphics/GLMeshRenderer.cpp
  src/graphics/GLMeshRendererBase.cpp
  src/graphics/GLPoints3.cpp
  src/graphics/GLPoints3Renderer.cpp
  src/graphics/GLProgram.cpp
  src/graphics/GLRenderer.cpp
  src/graphics/GLRenderWindow2.cpp
  src/graphics/GLRenderWindow3.cpp
  src/graphics/GLTextureFramebuffer.cpp
  src/graphics/GLWindow.cpp
  src/graphics/Image.cpp
  src/graphics/Light.cpp
  src/graphics/Primitive.cpp
  src/graphics/PrimitiveBVH.cpp
  src/graphics/PrimitiveMapper.cpp
  src/graphics/Renderer.cpp
  src/graphics/SceneEditor.cpp
  src/graphics/SceneWindowBase.cpp
  src/graphics/Shape.cpp
  src/graphics/TransformableObject.cpp
  src/graphics/TriangleMeshMapper.cpp
  src/graphics/TriangleMeshShape.cpp
  src/utils/MeshReader.cpp
  src/utils/MeshWriter.cpp
)

find_package(OpenGL REQUIRED)

if(WIN32)
  find_library(glfw3_LIB glfw3 ${CMAKE_CURRENT_LIST_DIR}/externals/lib)
  target_link_libraries(cg PUBLIC ${glfw3_LIB})
else()
  find_package(glfw3 REQUIRED)
  target_link_libraries(cg PUBLIC glfw)
endif()

if(NOT MSVC)
  target_compile_options(cg PRIVATE -Wno-narrowing)
endif()

target_include_directories(cg PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/include/
  ${CMAKE_CURRENT_LIST_DIR}/externals/include/
)

target_link_libraries(cg PUBLIC OpenGL::GL)

set(CG_LIB_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set_target_properties(cg PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${CG_LIB_OUTPUT_DIR}
  DEBUG_POSTFIX "D"
)
foreach(CG_OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
  string(TOUPPER ${CG_OUTPUTCONFIG} CG_OUTPUTCONFIG_UPPER)
  set_target_properties(cg PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY_${CG_OUTPUTCONFIG_UPPER} ${CG_LIB_OUTPUT_DIR}
  )
endforeach()

include(CheckIPOSupported)
check_ipo_supported(RESULT CG_IPO_SUPPORTED OUTPUT CG_IPO_ERROR)
if(CG_IPO_SUPPORTED)
  set_target_properties(cg PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()

if(MSVC)
  target_compile_options(cg PRIVATE /W3 /sdl /permissive-)
  # RuntimeLibrary MultiThreaded(Debug)DLL
  set_target_properties(cg PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
  )
  set_source_files_properties(
    src/utils/MeshReader.cpp
    src/utils/MeshWriter.cpp
    PROPERTIES COMPILE_DEFINITIONS "_CRT_SECURE_NO_WARNINGS"
  )
endif()

if(USE_CUDA)
  find_package(CUDA REQUIRED)
  # _USE_CUDA must be propagated
  target_compile_definitions(cg PUBLIC _USE_CUDA)
  target_link_libraries(cg PUBLIC CUDA::cudart CUDA::cuda_driver)
endif()
