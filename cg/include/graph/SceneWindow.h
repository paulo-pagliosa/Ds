//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2020, 2023 Paulo Pagliosa.                        |
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
// OVERVIEW: SceneWindow.h
// ========
// Class definition for generic graph scene window.
//
// Author: Paulo Pagliosa
// Last revision: 14/07/2023

#ifndef __GraphSceneWindow_h
#define __GraphSceneWindow_h

#include "graph/SceneObjectBuilder.h"
#include "graphics/SceneWindowBase.h"
#include <typeinfo>
#include <unordered_map>

namespace cg::graph
{ // begin namespace cg::graph


/////////////////////////////////////////////////////////////////////
//
// SceneWindow: generic graph scene window class
// ===========
class SceneWindow: public SceneWindowBase, public SceneObjectBuilder
{
public:
  template <typename W = SceneWindow, typename C = SharedObject>
  using InspectFunction = void (*)(W&, C&);

  template <typename W, typename C>
  void registerInspectFunction(InspectFunction<W, C> function)
  {
    assert(function != nullptr);
    _inspectFunctions[typeid(C).hash_code()] = (InspectFunction<>)function;
  }

  Scene* scene() const
  {
    return (Scene*)editor()->scene();
  }

protected:
  enum class ViewMode
  {
    Editor = 0,
    Renderer = 1
  };

  ViewMode _viewMode{};
  bool _showHierarchy{true};
  bool _showInspector{true};
  bool _showAssets{true};
  struct
  {
    bool hierarchy : 1;
    bool sceneObjects : 1;

  } _editFlags{};

  SceneWindow(const char* title, int width, int height):
    SceneWindowBase{title, width, height}
  {
    registerInspectFunction(inspectCamera);
    registerInspectFunction(inspectLight);
    registerInspectFunction(inspectPrimitive);
  }

  void setEditFlags(bool value)
  {
    _editFlags.hierarchy = _editFlags.sceneObjects = value;
  }

  auto editHierarchy() const
  {
    return _editFlags.hierarchy;
  }

  auto editSceneObjects() const
  {
    return _editFlags.sceneObjects;
  }

  virtual Scene* makeNewScene() const;

  void setScene(Scene&);

  void render() override;
  bool onMouseLeftPress(int, int) override;
  bool onKeyPress(int, int) override;

  virtual void renderScene();
  virtual void createObjectMenu();
  virtual Component* addComponentMenu(const SceneObject&);

  void drawSelectedObject(const SceneObject&);
  void drawComponents(const SceneObject&);

  virtual SceneObject* pickObject(int, int) const;
  SceneObject* pickObject(SceneObject*, const Ray3f&, float&) const;

  void hierarchyWindow(const char* = "Hierarchy");
  void inspectorWindow(const char* = "Inspector");

  void assetWindow()
  {
    if (_showAssets)
    {
      ImGui::Begin("Assets");
      assetPanels();
      ImGui::End();
    }
  }

  virtual void assetPanels();
  virtual void materialPanel();
  virtual void meshPanel();

  void inspectMaterial(Primitive&) const;

  static void inspectTransform(Transform&);
  static void inspectCamera(SceneWindow&, CameraProxy&);
  static void inspectLight(SceneWindow&, LightProxy&);
  static void inspectPrimitive(SceneWindow&, TriangleMeshProxy&);

private:
  using InspectMap = std::unordered_map<size_t, InspectFunction<>>;

  SceneNode _currentNode{};
  InspectMap _inspectFunctions;

  SceneBase* makeScene() final;

  void createObjectButton();
  bool treeNode(SceneNode, ImGuiTreeNodeFlags);
  bool deleteObjectPopup(SceneObject&);
  bool dropSceneObject(SceneObject&);
  bool objectHierarchy(SceneObject&);
  void addComponentButton(SceneObject&);

  auto inspectFunction(Component& component)
  {
    return _inspectFunctions[typeid(component).hash_code()];
  }

  void inspectSceneObject(SceneObject&);
  void inspectScene();
  void inspectCurrentNode();
  void inspectComponent(Component&);

}; // SceneWindow

} // end namespace cg::graph

#endif // __GraphSceneWindow_h
