//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2022 Paulo Pagliosa.                        |
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
// Last revision: 19/02/2022

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
  NameableObject{name},
  _scene{&scene}
{
  _flags.movable = movable;
  (_parent = scene.root())->_children.insert(this);
  _components.add(makeUse(&_transform));
  _transform._sceneObject = this;
}

SceneObject::~SceneObject()
{
  for (auto& component : _components)
    component->beforeRemoved();
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
    if (!component->canBeSiblingOf(newComponent))
      return false;
  return true;
}

Component*
SceneObject::insertComponent(Component* newComponent)
{
  if (newComponent == nullptr)
    return nullptr;

  // If the component cannot be added, then it must be deleted,
  // but only in case its reference count is zero
  Reference<Component> dummy{newComponent};

  if (!canAddComponent(newComponent))
    return nullptr;
  _components.add(newComponent);
  newComponent->_sceneObject = this;
  newComponent->afterAdded();
  // Set the transform changed flag to force component updating
  _transform.changed = true;
  newComponent->update();
  _transform.changed = false;
  return newComponent;
}

bool
SceneObject::removeComponent(const char* typeName)
{
  for (auto& component : _components)
    if (component->_erasable && component->_typeName == typeName)
    {
      component->beforeRemoved();
      component->_sceneObject = nullptr;
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

void
SceneObject::transformChanged()
{
  // Iterate the object components skipping its transform
  for (auto end = _components.end(), cit = ++_components.begin(); cit != end;)
    static_cast<Component*>(*cit++)->update();
  _transform.changed = false;
  for (auto& child : children())
    child._transform.update();
}

void
SceneObject::setVisible(bool value)
{
  if (value == _flags.visible)
    return;
  _flags.visible = value;
  for (auto end = _components.end(), cit = ++_components.begin(); cit != end;)
    static_cast<Component*>(*cit++)->setVisible(value);
  for (auto& child : children())
    child.setVisible(value);
}

namespace
{ // begin namespace

inline const char*
newSceneName()
{
  constexpr auto maxLen = 16;
  static char sceneName[maxLen];
  static int sceneId;

  snprintf(sceneName, maxLen, "Scene %d", ++sceneId);
  return sceneName;
}

} // end namespace

Scene::Scene(const char* name): // declared in Scene.h
  NameableObject{!*name ? newSceneName() : name},
  _root{*this}
{
  SceneObject::makeUse(&_root);
  _root.setName("root");
}

} // end namespace graph

} // end namespace cg
