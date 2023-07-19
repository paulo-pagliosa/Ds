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
// OVERVIEW: SceneObject.h
// ========
// Class definition for scene object.
//
// Author: Paulo Pagliosa
// Last revision: 19/07/2023

#ifndef __SceneObject_h
#define __SceneObject_h

#include "core/List.h"
#include "core/ObjectList.h"
#include "graph/Component.h"
#include "graph/SceneNode.h"
#include "graph/Transform.h"
#include <cassert>

namespace cg::graph
{ // begin namespace cg::graph

class Scene;
class SceneObject;

using SceneObjectAllocator = BlockAllocator<SceneObject, 32>;


/////////////////////////////////////////////////////////////////////
//
// SceneObject: scene object class
// ===========
class SceneObject: public NameableObject,
  public ObjectListNode<SceneObject, SceneObjectAllocator>
{
public:
  /// Constructs an empty scene object.
  static auto New(Scene& scene, const char* name = "")
  {
    return new SceneObject{scene, name};
  }

  ~SceneObject() override;

  /// Returns the scene which this scene object belong to.
  auto scene() const
  {
    return _scene;
  }

  /// Returns the parent of this scene object.
  auto parent() const
  {
    return _parent;
  }

  /// Sets the parent of this scene object.
  void setParent(SceneObject* parent);

  /// Returns the transform of this scene object.
  auto transform() const
  {
    return &_transform;
  }

  auto transform()
  {
    return &_transform;
  }

  SceneObject* addChild(SceneObject* child);

  auto makeChild(const char* name)
  {
    assert(name != nullptr);
    return addChild(New(*_scene, name));
  }

  bool removeChild(SceneObject* child);

  auto childrenCount() const
  {
    return (int)_children.size();
  }

  const auto& children() const
  {
    return _children;
  }

  auto& children()
  {
    return _children;
  }

  template <typename C>
  auto addComponent(C* component)
  {
    return (C*)insertComponent(component);
  }

  bool removeComponent(const char* typeName);

  auto componentCount() const
  {
    return (int)_components.size();
  }

  const auto& components() const
  {
    return _components;
  }

  auto& components()
  {
    return _components;
  }

  Component* findComponent(const char* typeName) const;

  /// Returns true if this scene object is movable.
  auto movable() const
  {
    return _flags.movable;
  }

  /// Returns true if this scene object is visible.
  auto visible() const
  {
    return _flags.visible;
  }

  void setVisible(bool value);

  /// Returns true if this scene object is selected.
  auto selected() const
  {
    return _flags.selected;
  }

  /// Returns true if this scene object is selectable.
  auto selectable() const
  {
    return _flags.selectable;
  }

  void setSelected(bool value)
  {
    _flags.selected = value;
  }

  void transformChanged();

private:
  using SceneObjects = ObjectList<SceneObject>;
  using Components = List<Reference<Component>>;

  Scene* _scene;
  SceneObject* _parent;
  Transform _transform;
  SceneObjects _children;
  Components _components;
  mutable struct
  {
    bool movable : 1;
    bool visible : 1;
    bool selectable : 1;
    bool selected : 1;

  } _flags{true, true, true, false};

  SceneObject(Scene& scene, const char* name, bool movable = true);

  SceneObject(Scene& scene):
    _scene{&scene},
    _parent{}
  {
    // do nothing
  }

  void changeParent(SceneObject*);
  Component* insertComponent(Component*);
  void makeComponentAttachments(Component*);
  void releaseComponentAttachments(Component*);

  bool canAddComponent(Component*) const;

  friend class Scene;
  friend class SceneEditor;

}; // SceneObject

/// Returns the transform of a component.
inline Transform*
Component::transform() const // declared in Component.h
{
  return sceneObject()->transform();
}

/// Returns the parent of a transform.
inline Transform*
Transform::parent() const // declared in Transform.h
{
  if (auto p = sceneObject()->parent())
    return p->transform();
  return nullptr;
}

} // end namespace cg::graph

#endif // __SceneObject_h
