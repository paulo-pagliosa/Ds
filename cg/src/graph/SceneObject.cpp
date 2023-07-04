//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2023 Paulo Pagliosa.                        |
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
// Last revision: 03/07/2023

#include "graph/Scene.h"

namespace cg::graph
{ // begin namespace cg::graph


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
SceneObject::canAddComponent(Component* component) const
{
  // Iterate the object components skipping its transform
  for (auto end = _components.end(), cit = ++_components.begin(); cit != end;)
    if (!static_cast<Component*>(*cit++)->canAdd(component))
      return false;
  return true;
}

inline void
SceneObject::makeComponentAttachments(Component* component) const
{
  // Iterate the object components skipping its transform
  for (auto end = _components.end(), cit = ++_components.begin(); cit != end;)
    if (auto c = *cit++; c.get() != component)
      // REMARK: connections between two components are not
      // bidirectional to avoid circular references
      if (!c->tryConnectingTo(component))
        component->tryConnectingTo(c);
}

Component*
SceneObject::insertComponent(Component* component)
{
  if (component == nullptr)
    return nullptr;

  // If the component cannot be added, then it must be deleted,
  // but only in case its reference count is zero
  Reference<Component> dummy{component};

  if (!canAddComponent(component))
    return nullptr;
  _components.add(component);
  component->_sceneObject = this;
  component->afterAdded();
  makeComponentAttachments(component);
  // Set the transform changed flag to force component updating
  _transform.setChanged(true);
  component->transformChanged();
  _transform.setChanged(false);
  return component;
}

inline void
SceneObject::releaseComponentAttachments(Component* component)
{
  // Iterate the object components skipping its transform
  for (auto end = _components.end(), cit = ++_components.begin(); cit != end;)
    if (auto c = *cit++; c.get() != component)
       if (!c->tryDisconnectingFrom(component))
         component->tryDisconnectingFrom(c);
}

bool
SceneObject::removeComponent(const char* typeName)
{
  for (auto& component : _components)
    if (component->erasable() && component->_typeName == typeName)
    {
      releaseComponentAttachments(component);
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
  for (const auto& c : _components)
    if (c->_typeName == typeName)
      return c;
  return nullptr;
}

void
SceneObject::transformChanged()
{
  // Iterate the object components skipping its transform
  for (auto end = _components.end(), cit = ++_components.begin(); cit != end;)
    static_cast<Component*>(*cit++)->transformChanged();
  _transform.setChanged(false);
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

inline bool
empty(const char* const s)
{
  return !s || !*s;
}

} // end namespace

Scene::Scene(const char* const name): // declared in Scene.h
  _root{*this}
{
  setName(empty(name) ? newSceneName() : name);
  SceneObject::makeUse(&_root);
  _root.setName("root");
}

} // end namespace cg::graph
