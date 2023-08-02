//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2022, 2023 Paulo Pagliosa.                        |
//|                                                                 |
//| This software is provided 'as-is', without any express or       |
//| implied warranty. In no event will the authors be held liable   |
//| for any damages arising from the use of this software.          |
//|                                                                 |
//| Permission is granted to anyone to use this software for any    |
//| purpose, including commercial applications, and to alter it and |
//| redistribute it freely, subject to the following restrictions:  |
//|                                                                 |
//| 1. The origin of this software must not be misrepresented; you  |
//| must not claim that you wrote the original software. If you use |
//| this software in a product, an acknowledgment in the product    |
//| documentation would be appreciated but is not required.         |
//|                                                                 |
//| 2. Altered source versions must be plainly marked as such, and  |
//| must not be misrepresented as being the original software.      |
//|                                                                 |
//| 3. This notice may not be removed or altered from any source    |
//| distribution.                                                   |
//|                                                                 |
//[]---------------------------------------------------------------[]
//
// OVERVIEW: SceneObjectBuilder.h
// ========
// Class definition for scene object builder.
//
// Author: Paulo Pagliosa
// Last revision: 01/08/2023

#ifndef __SceneObjectBuilder_h
#define __SceneObjectBuilder_h

#include "graph/CameraProxy.h"
#include "graph/LightProxy.h"
#include "graph/PrimitiveProxy.h"
#include "graph/Scene.h"

namespace cg::graph
{ // begin namespace cg::graph


/////////////////////////////////////////////////////////////////////
//
// SceneObjectBuilder: scene object builder class
// ==================
class SceneObjectBuilder
{
public:
  Scene* scene() const
  {
    return _scene;
  }

  void setScene(Scene&);

  SceneObject* createEmptyObject();
  SceneObject* createCameraObject(float aspect = 1, const char* = "");
  SceneObject* createLightObject(Light::Type, const char* = "");
  SceneObject* createPrimitiveObject(const TriangleMesh&, const std::string&);

  SceneObject* createObject(const char* name, Component* component)
  {
    assert(name != nullptr);

    auto object = SceneObject::New(*_scene, name);

    object->addComponent(component);
    return object;
  }

protected:
  Reference<Scene> _scene;
  uint32_t _objectId;
  uint32_t _cameraId;
  uint32_t _lightId;
  uint32_t _primitiveId;

  auto makePrimitive(const TriangleMesh& mesh, const std::string& meshName)
  {
    return TriangleMeshProxy::New(mesh, meshName);
  }

}; // SceneObjectBuilder

} // end namespace cg::graph

#endif // __SceneObjectBuilder_h
