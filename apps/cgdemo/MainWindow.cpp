//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2022 Orthrus Group.                               |
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
// OVERVIEW: MainWindow.cpp
// ========
// Source file for cg demo main window.
//
// Author: Paulo Pagliosa
// Last revision: 31/01/2022

#include "geometry/MeshSweeper.h"
#include "graphics/Application.h"
#include "MainWindow.h"

MeshMap MainWindow::_defaultMeshes;

inline void
MainWindow::buildDefaultMeshes()
{
  _defaultMeshes["Box"] = GLGraphics3::box();
  _defaultMeshes["Sphere"] = GLGraphics3::sphere();
  _defaultMeshes["Cone"] = GLGraphics3::cone();
  //_defaultMeshes["Plane"] = GLGraphics3::quad();
}

void
MainWindow::initializeScene()
{
  auto s = createCameraObject("Main Camera");

  s->transform()->setLocalPosition({0, 0, 10});
  s = createLightObject(Light::Type::Directional, "Directional Light");
  s->transform()->setLocalEulerAngles({50, 30, 0});
  createDefaultPrimitiveObject("Box");
}

void
MainWindow::beginInitialize()
{
  Assets::initialize();
  buildDefaultMeshes();
  Assets::meshes().insert(_defaultMeshes.begin(), _defaultMeshes.end());

  constexpr auto ffn = "fonts/Roboto-Regular.ttf";
  auto fonts = ImGui::GetIO().Fonts;

  fonts->AddFontFromFileTTF(Application::assetFilePath(ffn).c_str(), 16);
}

void
MainWindow::createObjectMenu()
{
  if (ImGui::BeginMenu("3D Object"))
  {
    if (ImGui::MenuItem("Box"))
      createDefaultPrimitiveObject("Box");
    if (ImGui::MenuItem("Sphere"))
      createDefaultPrimitiveObject("Sphere");
    if (ImGui::MenuItem("Cone"))
      createDefaultPrimitiveObject("Cone");
    //if (ImGui::MenuItem("Plane"))
      //createDefaultPrimitiveObject("Plane");
    ImGui::EndMenu();
  }
}

Component*
MainWindow::addComponentMenu()
{
  Component* component = nullptr;

  if (ImGui::BeginMenu("3D Object"))
  {
    if (ImGui::MenuItem("Box"))
      component = makeDefaultPrimitive("Box");
    if (ImGui::MenuItem("Sphere"))
      component = makeDefaultPrimitive("Sphere");
    if (ImGui::MenuItem("Cone"))
      component = makeDefaultPrimitive("Cone");
    //if (ImGui::MenuItem("Plane"))
      //component = makeDefaultPrimitive("Plane");
    ImGui::EndMenu();
  }
  return component;
}

inline void
MainWindow::fileMenu()
{
  if (ImGui::BeginMenu("File"))
  {
    if (ImGui::MenuItem("New Scene"))
    {
      createScene();
    }
    if (ImGui::BeginMenu("Open"))
    {
      // TODO
      ImGui::EndMenu();
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Exit", "Alt+F4"))
    {
      shutdown();
    }
    ImGui::EndMenu();
  }
}

inline void
MainWindow::createMenu()
{
  if (ImGui::BeginMenu("Create"))
  {
    if (ImGui::MenuItem("Material"))
      createMaterial();
    ImGui::Separator();
    createObjectMenu();
    ImGui::EndMenu();
  }
}

inline bool
showStyleSelector(const char* label)
{
  static int style = 1;

  if (!ImGui::Combo(label, &style, "Classic\0Dark\0Light\0"))
    return false;
  switch (style)
  {
    case 0: ImGui::StyleColorsClassic();
      break;
    case 1: ImGui::StyleColorsDark();
      break;
    case 2: ImGui::StyleColorsLight();
      break;
  }
  return true;
}

inline void
MainWindow::showOptions()
{
  ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.6f);
  showStyleSelector("Color Theme##Selector");
  ImGui::colorEdit3("Selected Wireframe", _selectedWireframeColor);
  ImGui::PopItemWidth();
}

inline void
MainWindow::mainMenu()
{
  if (ImGui::BeginMainMenuBar())
  {
    fileMenu();
    createMenu();
    if (ImGui::BeginMenu("View"))
    {
      static const char* viewLabels[]{"Editor", "Ray Tracer"};

      if (ImGui::BeginCombo("View", viewLabels[(int)_viewMode]))
      {
        for (auto i = 0; i < IM_ARRAYSIZE(viewLabels); ++i)
        {
          if (ImGui::Selectable(viewLabels[i], _viewMode == (ViewMode)i))
            _viewMode = (ViewMode)i;
        }
        ImGui::EndCombo();
        // TODO: change mode only if scene has changed
        if (_viewMode == ViewMode::Editor)
          _image = nullptr;
      }
      ImGui::Separator();
      ImGui::MenuItem("Preview", nullptr, &_showPreview);
      ImGui::MenuItem("Assets Window", nullptr, &_showAssets);
      ImGui::MenuItem("Editor View Settings", nullptr, &_showEditorView);
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Tools"))
    {
      if (ImGui::BeginMenu("Options"))
      {
        showOptions();
        ImGui::EndMenu();
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}

void
MainWindow::gui()
{
  mainMenu();
  if (_viewMode == ViewMode::Renderer)
    return;
  hierarchyWindow();
  inspectorWindow();
  editorView();
  assetsWindow();
}

void
MainWindow::renderScene()
{
  if (_viewMode != ViewMode::Renderer)
    return;

  auto camera = CameraProxy::current();

  if (nullptr == camera)
    camera = editor()->camera();
  if (_image == nullptr)
  {
    _image = new GLImage{width(), height()};
    if (_rayTracer == nullptr)
      _rayTracer = new RayTracer{*scene(), *camera};
    else
      _rayTracer->setCamera(*camera);
    _rayTracer->renderImage(*_image);
  }
  _image->draw(0, 0);
}

bool
MainWindow::onResize(int width, int height)
{
  _viewMode = ViewMode::Editor;
  _image = nullptr;
  return true;
}
