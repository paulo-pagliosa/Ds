//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2022 Orthrus Group.                         |
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
// OVERVIEW: SceneObject.cpp
// ========
// Source file for scene object.
//
// Author: Paulo Pagliosa
// Last revision: 19/01/2022

#include "graph/LightProxy.h"
#include "graph/Scene.h"

namespace cg
{ // begin namespace cg

namespace graph
{ // begin namespace graph


/////////////////////////////////////////////////////////////////////
//
// SceneObject implementation
// ===========
SceneObject::SceneObject(Scene& scene, const char* name, bool movable):
  SceneNode{name},
  _scene{&scene},
  _movable{movable}
{
  (_parent = scene.root())->_children.insert(this);
  addComponent(makeUse(&_transform));
}

inline void
SceneObject::changeParent(SceneObject* parent)
{
  (_parent = parent)->_children.insert(this);
  _transform.parentChanged();
}

void
SceneObject::setParent(SceneObject* object)
{
  const auto root = _scene->root();

  if (object == nullptr)
    object = root;
  else if (object->_scene != _scene)
  {
    _scene = object->_scene;
    changeParent(object);
    return;
  }
  if (object != _parent)
  {
    auto parent = object;

    for (;; object = object->_parent)
    {
      if (object == root)
      {
        changeParent(parent);
        return;
      }
      if (object == this)
        return;
    }
  }
}

SceneObject*
SceneObject::addChild(SceneObject* child)
{
  if (_children.insert(child) != nullptr && child->_parent != this)
  {
    makeUse(child);
    child->_parent = this;
  }
  return child;
}

bool
SceneObject::removeChild(SceneObject* child)
{
  if (_children.remove(child))
  {
    child->_parent = nullptr;
    release(child);
    return true;
  }
  return false;
}

inline bool
SceneObject::canAddComponent(Component* newComponent) const
{
  for (auto& component : _components)
    if (!component->acceptComponent(newComponent))
      return false;
  return true;
}

Component*
SceneObject::insertComponent(Component* newComponent)
{
  if (newComponent == nullptr)
    return nullptr;
  if (!canAddComponent(newComponent))
    return Component::release(newComponent), nullptr;
  _components.add(newComponent);
  if (auto proxy = dynamic_cast<LightProxy*>(newComponent))
    _scene->addLight(proxy->light());
  newComponent->_sceneObject = this;
  return newComponent;
}

bool
SceneObject::removeComponent(const char* typeName)
{
  for (auto& component : _components)
    if (component->_erasable && component->_typeName == typeName)
    {
      component->_sceneObject = nullptr;
      if (auto proxy = dynamic_cast<LightProxy*>(component.get()))
        _scene->removeLight(proxy->light());
      _components.remove(component);
      return true;
    }
  return false;
}

Component*
SceneObject::findComponent(const char* typeName) const
{
  for (const auto& component : _components)
    if (component->_typeName == typeName)
      return component;
  return nullptr;
}

namespace
{ // begin namespace

inline const char*
newSceneName()
{
  static int nextSceneId;
  static char name[20];

  snprintf(name, 20, "Scene %d", ++nextSceneId);
  return name;
}

} // end namespace

Scene::Scene(const char* name): // declared in Scene.h
  NameableObject{name == "" ? newSceneName() : name},
  _root{*this}
{
  SceneObject::makeUse(&_root);
  _root.setName("root");
}

} // end namespace graph

} // end namespace cg
