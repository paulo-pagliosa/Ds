//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2020, 2022 Orthrus Group.                         |
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
// Class definition for scene window base.
//
// Author: Paulo Pagliosa
// Last revision: 25/01/2022

#ifndef __SceneWindow_h
#define __SceneWindow_h

#include "geometry/Ray.h"
#include "graph/SceneEditor.h"
#include "graphics/GLWindow.h"
#include <typeinfo>
#include <unordered_map>

namespace cg
{ // begin namespace cg

namespace graph
{ // begin namespace graph

class CameraProxy;
class LightProxy;


/////////////////////////////////////////////////////////////////////
//
// SceneWindow: scene window base class
// ===========
class SceneWindow abstract: public GLWindow
{
public:
  template <typename C = SharedObject>
  using InspectFunction = void (*)(C&);

  Scene* scene() const
  {
    return _scene;
  }

  GLRenderer* renderer() const
  {
    return _editor;
  }

  template <typename C>
  void registerInspectFunction(InspectFunction<C> function)
  {
    assert(function != nullptr);
    _inspectMap[typeid(C).hash_code()] = (InspectFunction<>)function;
  }

protected:
  enum class ViewMode
  {
    Editor = 0,
    Renderer = 1
  };

  Color _selectedWireframeColor{255, 102, 0};
  ViewMode _viewMode{ViewMode::Editor};
  bool _showEditorView{true};
  bool _showAssets{true};

  SceneWindow(const char* title, int width, int height):
    GLWindow{title, width, height}
  {
    registerInspectFunction<CameraProxy>(inspectCamera);
    registerInspectFunction<LightProxy>(inspectLight);
    registerInspectFunction<TriangleMeshProxy>(inspectPrimitive);
  }

  void initialize() override;
  void render() override;

  virtual void beginInitialize();
  virtual void endInitialize();
  virtual void initializeScene();
  virtual void renderScene();
  virtual void createObjectMenu();
  virtual Component* addComponentMenu();
  virtual bool onResize(int, int);

  Scene* createScene();

  SceneObject* createEmptyObject();
  SceneObject* createCameraObject(const char* = nullptr);
  SceneObject* createLightObject(Light::Type, const char* = nullptr);
  SceneObject* createPrimitiveObject(const TriangleMesh&, const std::string&);

  SceneObject* createObject(const char* name, Component* component)
  {
    assert(name != nullptr);
    
    auto object = SceneObject::New(*_scene, name);

    object->addComponent(component);
    return object;
  }

  Material* createMaterial();

  auto makePrimitive(const TriangleMesh& mesh, const std::string& meshName)
  {
    return TriangleMeshProxy::New(mesh, meshName);
  }

  void drawSelectedObject(const SceneObject&);
  void drawComponents(const SceneObject&);

  virtual SceneObject* pickObject(int, int) const;
  SceneObject* pickObject(SceneObject*, const Ray3f&, float&) const;

  SceneEditor* editor() const
  {
    return _editor;
  }

  void hierarchyWindow(const char* = "Hierarchy");
  void inspectorWindow(const char* = "Inspector");
  void editorView();
  void assetsWindow();

  void showErrorMessage(const char*) const;

  static void inspectTransform(Transform&);
  static void inspectCamera(Camera&);
  static void inspectCamera(CameraProxy&);
  static void inspectLight(Light&);
  static void inspectLight(LightProxy&);
  static void inspectMaterial(Material&);
  static void inspectPrimitive(TriangleMeshProxy&);

private:
  using InspectMap = std::unordered_map<size_t, InspectFunction<>>;

  enum class MoveBits
  {
    Left = 1,
    Right = 2,
    Forward = 4,
    Back = 8,
    Up = 16,
    Down = 32
  };

  enum class DragBits
  {
    Rotate = 1,
    Pan = 2
  };

  Reference<Scene> _scene;
  Reference<SceneEditor> _editor;
  SceneNode* _currentNode{};
  Flags<MoveBits> _moveFlags{};
  Flags<DragBits> _dragFlags{};
  InspectMap _inspectMap;

  struct
  {
    int px, py;
    int cx, cy;

  } _mouse;

  bool windowResizeEvent(int, int) override;
  bool scrollEvent(double, double) override;
  bool mouseButtonInputEvent(int, int, int) override;
  bool mouseMoveEvent(double, double) override;
  bool keyInputEvent(int, int, int) override;

  void createObjectButton();
  bool treeNode(SceneNode&, ImGuiTreeNodeFlags);
  bool deleteObjectPopup(SceneObject&);
  bool objectHierarchy(SceneObject&);
  void addComponentButton(SceneObject&);

  auto inspectFunction(Component& component)
  {
    return _inspectMap[typeid(component).hash_code()];
  }

  void inspectSceneObject(SceneObject&);
  void inspectScene();
  void inspectCurrentNode();
  void inspectComponent(Component&);

  Ray3f makeRay(int, int) const;

}; // SceneWindow

} // end namespace graph

} // end namespace cg

namespace ImGui
{ // begin namespace ImGui

using namespace cg;

void objectNameInput(NameableObject& object);

inline bool
dragVec2(const char* label, vec2f& v, float min = 0, float max = 0)
{
  return ImGui::DragFloat2(label, &v.x, 0.01f, min, max, "%.2g");
}

inline bool
dragVec2(const char* label, vec2f& v, vec2f r)
{
  return ImGui::SliderFloat2(label, &v.x, r.x, r.y, "%.2g");
}

inline bool
dragVec3(const char* label, vec3f& v, float min = 0, float max = 0)
{
  // TODO: replace 0.1f
  return ImGui::DragFloat3(label, &v.x, 0.1f, min, max, "%.2g");
}

inline bool
dragVec3(const char* label, vec3f& v, vec2f r)
{
  return ImGui::SliderFloat3(label, &v.x, r.x, r.y, "%.2g");
}

inline bool
colorEdit3(const char* label, Color& color)
{
  return ImGui::ColorEdit3(label, &color.r);
}

} // end namespace ImGui

#endif // __SceneWindow_h
