cmake_minimum_required(VERSION 3.25)
project(Ds LANGUAGES CXX CUDA C)

include(cmake/externals.cmake)
add_subdirectory(apps/cgdemo/reader)

set(cg_header_files
    # core directory
    cg/include/core/AllocableObject.h
    cg/include/core/Array.h
    cg/include/core/BlockAllocable.h
    cg/include/core/BlockAllocator.h
    cg/include/core/ContentHolder.h
    cg/include/core/Exception.h
    cg/include/core/Flags.h
    cg/include/core/Globals.h
    cg/include/core/List.h
    cg/include/core/ListBase.h
    cg/include/core/NameableObject.h
    cg/include/core/ObjectList.h
    cg/include/core/ObjectPool.h
    cg/include/core/SharedObject.h
    cg/include/core/SoA.h
    cg/include/core/StandardAllocator.h
    
    # debug directory
    cg/include/debug/AnimatedAlgorithm.h

    # geometry directory
    cg/include/geometry/Bounds2.h
    cg/include/geometry/Bounds3.h
    cg/include/geometry/BVH.h
    cg/include/geometry/Grid2.h
    cg/include/geometry/Grid3.h
    cg/include/geometry/GridBase.h
    cg/include/geometry/Index2.h
    cg/include/geometry/Index3.h
    cg/include/geometry/IndexList.h
    cg/include/geometry/Intersection.h
    cg/include/geometry/KNNHelper.h
    cg/include/geometry/Line.h
    cg/include/geometry/MeshSweeper.h
    cg/include/geometry/Octree.h
    cg/include/geometry/Point2.h
    cg/include/geometry/Point3.h
    cg/include/geometry/PointArray.h
    cg/include/geometry/PointGrid2.h
    cg/include/geometry/PointGrid3.h
    cg/include/geometry/PointGridBase.h
    cg/include/geometry/PointHolder.h
    cg/include/geometry/PointOctree.h
    cg/include/geometry/PointQuadtree.h
    cg/include/geometry/PointTreeBase.h
    cg/include/geometry/Quad.h
    cg/include/geometry/Quadtree.h
    cg/include/geometry/Ray.h
    cg/include/geometry/TreeBase.h
    cg/include/geometry/Triangle.h
    cg/include/geometry/TriangleMesh.h
    cg/include/geometry/TriangleMeshBVH.h

    # graph directory
    cg/include/graph/CameraProxy.h
    cg/include/graph/Component.h
    cg/include/graph/ComponentProxy.h
    cg/include/graph/LightProxy.h
    cg/include/graph/PrimitiveProxy.h
    cg/include/graph/Scene.h
    cg/include/graph/SceneNode.h
    cg/include/graph/SceneObject.h
    cg/include/graph/SceneObjectBuilder.h
    cg/include/graph/SceneWindow.h
    cg/include/graph/Transform.h

    # graphics directory
    cg/include/graphics/Actor.h
    cg/include/graphics/Application.h
    cg/include/graphics/Assets.h
    cg/include/graphics/AssetFolder.h
    cg/include/graphics/Camera.h
    cg/include/graphics/Color.h
    cg/include/graphics/GLBuffer.h
    cg/include/graphics/GLFramebuffer.h
    cg/include/graphics/GLGraphics2.h
    cg/include/graphics/GLGraphics3.h
    cg/include/graphics/GLGraphicsBase.h
    cg/include/graphics/GLImage.h
    cg/include/graphics/GLMesh.h
    cg/include/graphics/GLMeshRenderer.h
    cg/include/graphics/GLProgram.h
    cg/include/graphics/GLRenderer.h
    cg/include/graphics/GLRendererBase.h
    cg/include/graphics/GLRenderWindow2.h
    cg/include/graphics/GLRenderWindow3.h
    cg/include/graphics/GLTextureFramebuffer.h
    cg/include/graphics/GLWindow.h
    cg/include/graphics/Image.h
    cg/include/graphics/Light.h
    cg/include/graphics/Material.h
    cg/include/graphics/Primitive.h
    cg/include/graphics/PrimitiveBVH.h
    cg/include/graphics/PrimitiveMapper.h
    cg/include/graphics/Renderer.h
    cg/include/graphics/SceneBase.h
    cg/include/graphics/SceneEditor.h
    cg/include/graphics/SceneWindowBase.h
    cg/include/graphics/Shape.h
    cg/include/graphics/TransformableObject.h
    cg/include/graphics/TriangleMeshMapper.cpp
    cg/include/graphics/TriangleMeshMapper.h
    cg/include/graphics/TriangleMeshShape.h

    # math directory
    cg/include/math/Matrix3x3.h
    cg/include/math/Matrix4x4.h
    cg/include/math/Quaternion.h
    cg/include/math/Real.h
    cg/include/math/RealLimits.h
    cg/include/math/Vector2.h
    cg/include/math/Vector3.h
    cg/include/math/Vector4.h

    # utils directory
    cg/include/utils/MeshReader.h
    cg/include/utils/Stopwatch.h
)

set(cg_source_files
    # core files
    cg/src/core/BlockAllocator.cpp
    cg/src/core/Exception.cpp
    cg/src/core/NameableObject.cpp

    # debug directory
    cg/src/debug/AnimatedAlgorithm.cpp

    # geometry directory
    cg/src/geometry/BVH.cpp
    cg/src/geometry/MeshSweeper.cpp
    cg/src/geometry/TriangleMesh.cpp
    cg/src/geometry/TriangleMeshBVH.cpp

    # graph directory
    cg/src/graph/CameraProxy.cpp
    cg/src/graph/Component.cpp
    cg/src/graph/LightProxy.cpp
    cg/src/graph/PrimitiveProxy.cpp
    cg/src/graph/SceneObject.cpp
    cg/src/graph/SceneObjectBuilder.cpp
    cg/src/graph/SceneWindow.cpp
    cg/src/graph/Transform.cpp

    # graphics directory
    cg/src/graphics/Application.cpp
    cg/src/graphics/Assets.cpp
    cg/src/graphics/AssetFolder.cpp
    cg/src/graphics/Camera.cpp
    cg/src/graphics/Color.cpp
    cg/src/graphics/GLFramebuffer.cpp
    cg/src/graphics/GLGraphics2.cpp
    cg/src/graphics/GLGraphics3.cpp
    cg/src/graphics/GLGraphicsBase.cpp
    cg/src/graphics/GLImage.cpp
    cg/src/graphics/GLMesh.cpp
    cg/src/graphics/GLMeshRenderer.cpp
    cg/src/graphics/GLProgram.cpp
    cg/src/graphics/GLRenderer.cpp
    cg/src/graphics/GLRendererBase.cpp
    cg/src/graphics/GLRenderWindow2.cpp
    cg/src/graphics/GLRenderWindow3.cpp
    cg/src/graphics/GLTextureFramebuffer.cpp
    cg/src/graphics/GLWindow.cpp
    cg/src/graphics/Image.cpp
    cg/src/graphics/Light.cpp
    cg/src/graphics/Primitive.cpp
    cg/src/graphics/PrimitiveBVH.cpp
    cg/src/graphics/PrimitiveMapper.cpp
    cg/src/graphics/Renderer.cpp
    cg/src/graphics/SceneEditor.cpp
    cg/src/graphics/SceneWindowBase.cpp
    cg/src/graphics/Shape.cpp
    cg/src/graphics/TransformableObject.cpp
    cg/src/graphics/TriangleMeshShape.cpp

    # utils directory
    cg/src/utils/MeshReader.cpp
)

add_library(ds_cg_lib STATIC ${cg_header_files} ${cg_source_files})
target_include_directories(ds_cg_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/cg/include)
set_target_properties(ds_cg_lib PROPERTIES CXX_EXTENSIONS OFF)
target_compile_features(ds_cg_lib PRIVATE cxx_std_20)
target_link_libraries(ds_cg_lib PUBLIC glfw ds_imgui gl3w)
add_library(ds::cg ALIAS ds_cg_lib)

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    add_subdirectory(apps)
endif()
