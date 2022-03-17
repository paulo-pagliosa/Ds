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
// OVERVIEW: SceneWindow.h
// ========
// Class definition for generic vis scene window.
//
// Author: Paulo Pagliosa
// Last revision: 16/03/2022

#ifndef __VisSceneWindow_h
#define __VisSceneWindow_h

#include "graph/SceneNode.h"
#include "graphics/SceneWindowBase.h"
#include "PolyDataMapper.h"
#include "PolyMeshMapper.h"
#include "TriCellMeshMapper.h"
#include "Scene.h"
#include <typeinfo>
#include <unordered_map>

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// SceneWindow: vis scene window
// ===========
class SceneWindow: public SceneWindowBase
{
public:
  using InspectFunction = void (*)(SceneWindow&, AbstractMapper&);

  template <typename M>
  void registerInspectFunction(InspectFunction function)
  {
    assert(function != nullptr);
    _inspectFunctions[typeid(M).hash_code()] = function;
  }

  Scene* scene() const
  {
    return (Scene*)editor()->scene();
  }

protected:
  bool _showHierarchy{true};
  bool _showInspector{true};

  SceneWindow(const char* title, int width, int height):
    SceneWindowBase{title, width, height}
  {
    registerInspectFunction<PolyDataMapper>(inspectPolyDataMapper);
    registerInspectFunction<TriCellMeshMapper>(inspectTriCellMeshMapper);
    registerInspectFunction<PolyMeshMapper>(inspectPolyMeshMapper);
  }

  void render() override;
  bool onPressKey(int) override;

  void hierarchyWindow(const char* = "Hierarchy");
  void inspectorWindow(const char* = "Inspector");

  void inspectScene();
  void inspectActor(Actor&);
  void inspectLight(Light&);
  void inspectMapper(PrimitiveMapper&);

  static void inspectPolyDataMapper(SceneWindow&, AbstractMapper&);
  static void inspectTriCellMeshMapper(SceneWindow&, AbstractMapper&);
  static void inspectPolyMeshMapper(SceneWindow&, AbstractMapper&);

private:
  using InspectMap = std::unordered_map<size_t, InspectFunction>;

  graph::SceneNode _currentNode;
  InspectMap _inspectFunctions;

  bool treeNode(graph::SceneNode, ImGuiTreeNodeFlags);
  void inspectCurrentNode();

  auto inspectFunction(AbstractMapper& mapper)
  {
    return _inspectFunctions[typeid(mapper).hash_code()];
  }

  SceneBase* makeScene() override;

}; // SceneWindow

} // end namespace cg::vis

#endif // __VisSceneWindow_h
