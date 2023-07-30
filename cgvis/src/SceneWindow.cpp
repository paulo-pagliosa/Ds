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
// OVERVIEW: SceneWindow.cpp
// ========
// Source file for generic vis scene window.
//
// Author: Paulo Pagliosa
// Last revision: 30/07/2023

#include "SceneWindow.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// SceneWindow implementation
// ===========
SceneBase*
SceneWindow::makeScene()
{
  auto scene = Scene::New();

  _currentNode = scene;
  return scene;
}

void
SceneWindow::render()
{
  SceneWindowBase::render();

  auto editor = this->editor();

  if (auto actor = _currentNode.as<Actor>())
  {
    editor->setLineColor(_selectedWireframeColor[0]);
    editor->drawBounds(actor->mapper()->bounds());
  }
  else if (auto light = _currentNode.as<Light>())
  {
    editor->drawLight(*light);
    editor->drawTransform(light->position(), light->rotation());
  }
}

bool
SceneWindow::onKeyPress(int key, int)
{
  if (key != GLFW_KEY_F || _currentNode.as<Scene>())
    return false;

  vec3f p;

  if (auto actor = _currentNode.as<Actor>())
    p = actor->mapper()->bounds().center();
  else
    p = _currentNode.as<Light>()->position();

  auto editor = this->editor();
  auto camera = editor->camera();
  auto d = camera->viewPlaneNormal();

  // TODO: set view angle/height and distance
  camera->setPosition(p + d * editor->orbitDistance());
  return true;
}

inline void
SceneWindow::inspectScene()
{
  const auto scene = this->scene();

  ImGui::objectNameInput(*scene);
  ImGui::Separator();
  if (ImGui::CollapsingHeader("Environment"))
  {
    ImGui::colorEdit3("Background", scene->backgroundColor);
    ImGui::colorEdit3("Ambient Light", scene->ambientLight);
  }
}

inline void
SceneWindow::inspectActor(Actor& actor)
{
  auto visible = actor.isVisible();

  ImGui::objectNameInput(actor);
  ImGui::SameLine();
  if (ImGui::Checkbox("###visible", &visible))
    actor.visible = visible;
  ImGui::Separator();
  inspectMapper(*actor.mapper());
}

inline void
SceneWindow::inspectLight(Light& light)
{
  ImGui::objectNameInput(light);
  ImGui::Separator();

  vec3f temp;

  temp = light.position();
  if (ImGui::dragVec3("Position", temp))
    light.setPosition(temp);
  temp = light.eulerAngles();
  if (ImGui::dragVec3("Rotation", temp))
    light.setEulerAngles(temp);
  ImGui::Separator();
  SceneWindowBase::inspectLight(light);
}

inline void
SceneWindow::inspectCurrentNode()
{
  if (_currentNode == nullptr)
    return;
  if (_currentNode.as<Scene>())
    inspectScene();
  else if (auto actor = _currentNode.as<Actor>())
    inspectActor(*actor);
  else if (auto light = _currentNode.as<Light>())
    inspectLight(*light);
}

inline ImU32
toImU32(const Color& c)
{
  return IM_COL32((int)(c.r * 255), (int)(c.g * 255), (int)(c.b * 255), 255);
}

inline void
SceneWindow::inspectMapper(PrimitiveMapper& primitiveMapper)
{
  auto mapper = asAbstractMapper(&primitiveMapper);

  if (mapper == nullptr || !ImGui::CollapsingHeader(mapper->name()))
    return;
  if (const auto lut = mapper->lookupTable())
  {
    ImGui::Checkbox("Use Color Map", &mapper->useVertexColors);
    if (mapper->useVertexColors)
    {
      const auto drawList = ImGui::GetWindowDrawList();
      const auto cp = ImGui::GetCursorScreenPos();
      const auto sx = ImGui::CalcItemWidth();
      const auto sy = 32.0f;
      const auto lutSamples = 8;
      const auto dx = sx / lutSamples;
      const auto dk = lut->size() / lutSamples;
      const auto y2 = cp.y + sy;
      auto x1 = cp.x;

      for (int i = 0, k = 0; i < lutSamples; ++i, k += dk)
      {
        auto c1 = toImU32((*lut)[k]);
        auto c2 = toImU32((*lut)[k + dk - 1]);
        auto x2 = x1 + dx;

        drawList->AddRectFilledMultiColor({x1, cp.y},
          {x2, y2},
          c1,
          c2,
          c2,
          c1);
        x1 = x2;
      }
      ImGui::SetCursorScreenPos({x1 + 4,
        cp.y + (sy - ImGui::GetTextLineHeight()) / 2});
      ImGui::TextUnformatted("Lookup Table");
      ImGui::SetCursorScreenPos({cp.x, y2 + 4});

      float range[2]{lut->minScalar(), lut->maxScalar()};

      ImGui::InputFloat2("Scalar Range",
        range,
        "%.3f",
        ImGuiInputTextFlags_ReadOnly);
      ImGui::Separator();
    }
  }
  ImGui::Checkbox("Draw Bounds", &mapper->drawBounds);
  if (auto function = inspectFunction(*mapper))
    function(*this, *mapper);
}

inline bool
SceneWindow::treeNode(graph::SceneNode node, ImGuiTreeNodeFlags flags)
{
  if (node == _currentNode)
    flags |= ImGuiTreeNodeFlags_Selected;

  auto open = ImGui::TreeNodeEx(node, flags, "%s", node->name());

  if (ImGui::IsItemClicked())
    _currentNode = node;
  return open;
}

void
SceneWindow::hierarchyWindow(const char* title)
{
  if (!_showHierarchy)
    return;
  assert(title != nullptr);

  auto scene = this->scene();
  ImGuiTreeNodeFlags flags{ImGuiTreeNodeFlags_OpenOnArrow};

  ImGui::Begin(title);
  if (treeNode(scene, flags))
  {
    flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    if (ImGui::TreeNode("Actors"))
    {
      for (auto actor : scene->actors())
        treeNode(actor, flags);
      ImGui::TreePop();
    }
    if (ImGui::TreeNode("Lights"))
    {
      for (auto light : scene->lights())
        treeNode(light, flags);
      ImGui::TreePop();
    }
    ImGui::TreePop();
  }
  ImGui::End();
}

void
SceneWindow::inspectorWindow(const char* title)
{
  if (!_showInspector)
    return;
  assert(title != nullptr);
  ImGui::Begin(title);
  inspectCurrentNode();
  ImGui::End();
}

void
SceneWindow::inspectPolyDataMapper(SceneWindow& window,
  AbstractMapper& mapper)
{
  // TODO: edit poly data properties
  /*
  if (auto m = dynamic_cast<PolyDataMapper*>(&mapper))
  {
    auto data = m->input();
    auto ps = data->pointSize();

    if (ImGui::DragFloat("Point Size", &ps, 1, 1, 20))
      data->setPointSize(ps);
    if (ImGui::CollapsingHeader("Colors"))
    {
      ImGui::colorEdit3("Point", data->pointColor);
      ImGui::colorEdit3("Line", data->lineColor);
      ImGui::colorEdit3("Triangle", data->triangleColor);
    }
  }
  */
}

void
SceneWindow::inspectTriCellMeshMapper(SceneWindow& window,
  AbstractMapper& mapper)
{
  if (auto m = dynamic_cast<TriCellMeshMapper*>(&mapper))
    if (ImGui::CollapsingHeader("Material"))
      inspectMaterial(*m->input()->material());
}

void
SceneWindow::inspectPolyMeshMapper(SceneWindow& window,
  AbstractMapper& mapper)
{
  // TODO: edit poly mesh color
  /*
  if (auto m = dynamic_cast<PolyMeshMapper*>(&mapper))
  {
    auto mesh = m->input();
    ImGui::colorEdit3("Color", mesh->instanceColor);
  }
  */
}

} // end namespace cg::vis
