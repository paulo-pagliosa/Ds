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
// OVERVIEW: SceneObjectBuilder.cpp
// ========
// Source file for scene object builder.
//
// Author: Paulo Pagliosa
// Last revision: 01/08/2023

#include "graph/SceneObjectBuilder.h"
#include "graphics/Assets.h"

namespace cg::graph
{ // begin namespace cg::graph

static inline auto
empty(const char* s)
{
  return s == nullptr || *s == 0;
}


/////////////////////////////////////////////////////////////////////
//
// SceneObjectBuilder implementation
// ==================
void
SceneObjectBuilder::setScene(Scene& scene)
{
  _scene = &scene;
  _objectId = _cameraId = _lightId = _primitiveId = 0;
}

SceneObject*
SceneObjectBuilder::createEmptyObject()
{
  auto object = SceneObject::New(*_scene);

  object->setName("Object %d", ++_objectId);
  return object;
}

SceneObject*
SceneObjectBuilder::createCameraObject(float aspect, const char* name)
{
  auto object = SceneObject::New(*_scene);

  if (!empty(name))
    object->setName(name);
  else
    object->setName("Camera %d", ++_cameraId);

  auto camera = new Camera{aspect};

  object->addComponent(CameraProxy::New(*camera));
  CameraProxy::setCurrent(camera);
  return object;
}

SceneObject*
SceneObjectBuilder::createLightObject(Light::Type type, const char* name)
{
  auto object = SceneObject::New(*_scene);

  if (!empty(name))
    object->setName(name);
  else
    object->setName("Light %d", ++_lightId);

  auto light = new Light;

  light->setType(type);
  object->addComponent(LightProxy::New(*light));;
  return object;
}

SceneObject*
SceneObjectBuilder::createPrimitiveObject(const TriangleMesh& mesh,
  const std::string& meshName)
{
  auto object = SceneObject::New(*_scene);

  object->setName("%s %d", meshName.c_str(), ++_primitiveId);
  object->addComponent(makePrimitive(mesh, meshName));
  return object;
}

} // end namespace cg::graph
