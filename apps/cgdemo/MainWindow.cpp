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
// OVERVIEW: MainWindow.cpp
// ========
// Source file for cg demo main window.
//
// Author: Paulo Pagliosa
// Last revision: 13/07/2023

#include "graphics/Application.h"
#include "graphics/AssetFolder.h"
#include "reader/SceneReader.h"
#include "SceneWriter.h"
#include "MainWindow.h"


/////////////////////////////////////////////////////////////////////
//
// MainWindow implementation
// ==========
MeshMap MainWindow::_defaultMeshes;

inline void
MainWindow::buildDefaultMeshes()
{
  _defaultMeshes["Box"] = GLGraphics3::box();
  _defaultMeshes["Sphere"] = GLGraphics3::sphere();
  _defaultMeshes["Cylinder"] = GLGraphics3::cylinder();
  _defaultMeshes["Cone"] = GLGraphics3::cone();
  _defaultMeshes["Plane"] = GLGraphics3::quad();
}

void
MainWindow::initializeScene()
{
  auto a = (float)width() / (float)height();
  auto s = createCameraObject(a, "Main Camera");

  s->transform()->setLocalPosition({0, 0, 10});
  s = createLightObject(Light::Type::Directional, "Directional Light");
  s->transform()->setLocalEulerAngles({50, 30, 0});
  createDefaultPrimitiveObject("Box");
}

void
MainWindow::beginInitialize()
{
  constexpr auto ffn = "fonts/Roboto-Regular.ttf";
  auto fonts = ImGui::GetIO().Fonts;

  fonts->AddFontFromFileTTF(Application::assetFilePath(ffn).c_str(), 16);
  buildDefaultMeshes();
  Assets::initialize();
  Assets::meshes().insert(_defaultMeshes.begin(), _defaultMeshes.end());
  _sceneFolder = AssetFolder::New("scenes/", ".scn");
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
    if (ImGui::MenuItem("Cylinder"))
      createDefaultPrimitiveObject("Cylinder");
    if (ImGui::MenuItem("Cone"))
      createDefaultPrimitiveObject("Cone");
    if (ImGui::MenuItem("Plane"))
      createDefaultPrimitiveObject("Plane");
    ImGui::EndMenu();
  }
}

Component*
MainWindow::addComponentMenu(const SceneObject&)
{
  Component* component = nullptr;

  if (ImGui::BeginMenu("3D Object"))
  {
    if (ImGui::MenuItem("Box"))
      component = makeDefaultPrimitive("Box");
    if (ImGui::MenuItem("Sphere"))
      component = makeDefaultPrimitive("Sphere");
    if (ImGui::MenuItem("Cylinder"))
      component = makeDefaultPrimitive("Cylinder");
    if (ImGui::MenuItem("Cone"))
      component = makeDefaultPrimitive("Cone");
    if (ImGui::MenuItem("Plane"))
      component = makeDefaultPrimitive("Plane");
    ImGui::EndMenu();
  }
  return component;
}

void
MainWindow::openSceneCommand()
{
  if (ImGui::BeginListBox("##SceneList",
    ImVec2{180, 8 * ImGui::GetTextLineHeightWithSpacing()}))
  {
    for (auto& file : _sceneFolder->files())
      if (ImGui::Selectable(file->filename().c_str()))
      {
        auto filename = (_sceneFolder->path() / file->name()).string();

        readScene(filename);
        ImGui::CloseCurrentPopup();
      }
    ImGui::EndListBox();
  }
}

void
MainWindow::readScene(const std::string& filename) try
{
  util::SceneReader reader;

  reader.setInput(filename);
  reader.execute();
  if (reader.scene() != nullptr)
  {
    SceneWindow::setScene(*reader.scene());

    auto& materials = Assets::materials();

    for (const auto& [name, m] : reader.materials)
      materials[name] = m;
  }
}
catch (const std::exception& e)
{
  puts(e.what());
}

void
MainWindow::saveScene()
{
  if (auto name = scene()->name(); *name)
  {
    auto filename = std::string{name} + ".scn";
    auto path = (_sceneFolder->path() / filename).string();

    util::SceneWriter{path.c_str()}.write(*scene());
    _sceneFolder->addFile(filename.c_str());
  }
}

inline void
MainWindow::fileMenu()
{
  if (ImGui::BeginMenu("File"))
  {
    if (ImGui::MenuItem("New Scene"))
      newScene();
    if (ImGui::BeginMenu("Open"))
    {
      openSceneCommand();
      ImGui::EndMenu();
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Save"))
      saveScene();
    ImGui::Separator();
    if (ImGui::MenuItem("Exit", "Alt+F4"))
    {
      shutdown();
    }
    ImGui::EndMenu();
  }
}

inline void
MainWindow::viewMenu()
{
  if (ImGui::BeginMenu("View"))
  {
    static const char* viewLabels[]{"Editor", "Ray Tracer"};

    if (ImGui::BeginCombo("View", viewLabels[(int)_viewMode]))
    {
      for (auto i = 0; i < IM_ARRAYSIZE(viewLabels); ++i)
        if (ImGui::Selectable(viewLabels[i], _viewMode == (ViewMode)i))
          _viewMode = (ViewMode)i;
      ImGui::EndCombo();
      // TODO: change mode only if scene has changed
      if (_viewMode == ViewMode::Editor)
        _image = nullptr;
    }
    ImGui::Separator();
    ImGui::MenuItem("Hierarchy Window", nullptr, &_showHierarchy);
    ImGui::MenuItem("Inspector Window", nullptr, &_showInspector);
    ImGui::MenuItem("Camera Preview", nullptr, &_showPreview);
    ImGui::MenuItem("Assets Window", nullptr, &_showAssets);
    ImGui::MenuItem("Editor View Settings", nullptr, &_showEditorView);
    ImGui::EndMenu();
  }
}

inline void
MainWindow::createMenu()
{
  ImGui::BeginDisabled(!editHierarchy());
  if (ImGui::BeginMenu("Create"))
  {
    if (ImGui::MenuItem("Material"))
      createMaterial();
    ImGui::Separator();
    createObjectMenu();
    ImGui::EndMenu();
  }
  ImGui::EndDisabled();
}

inline void
MainWindow::showOptions()
{
  ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.6f);
  ImGui::showStyleSelector("Color Theme##Selector");
  ImGui::colorEdit3("Selected Wireframe", _selectedWireframeColor[0]);
  ImGui::colorEdit3("Selected Children Wireframe", _selectedWireframeColor[1]);
  ImGui::PopItemWidth();
}

inline void
MainWindow::mainMenu()
{
  if (ImGui::BeginMainMenuBar())
  {
    fileMenu();
    createMenu();
    viewMenu();
    if (ImGui::BeginMenu("Ray Tracing"))
    {
      ImGui::DragInt("Max Recursion Level",
        &_maxRecursionLevel,
        1.0f,
        0,
        RayTracer::maxMaxRecursionLevel);
      ImGui::DragFloat("Min Weight",
        &_minWeight,
        0.01f,
        RayTracer::minMinWeight,
        1.0f);
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

  constexpr auto evw = (float)360;
  constexpr auto evh = (float)308;
  constexpr auto lft = (float)0;
  constexpr auto top = (float)23;
  auto w = (float)width();
  auto h = (float)height();
  auto evy = h - evh;

  // Hierarchy Window
  ImGui::SetNextWindowPos({lft, top});
  ImGui::SetNextWindowSize({evw, evy - top});
  hierarchyWindow();
  // Editor View
  ImGui::SetNextWindowPos({lft, evy});
  ImGui::SetNextWindowSize({evw, evh});
  editorView();

  constexpr auto iww = (float)420;
  auto rgt = w - iww;
  auto awh = h / 3;
  auto awy = h - awh;

  // Inspector Window
  ImGui::SetNextWindowPos({rgt, top});
  ImGui::SetNextWindowSize({iww, awy - top});
  inspectorWindow();
  // Assets Window
  ImGui::SetNextWindowPos({rgt, awy});
  ImGui::SetNextWindowSize({iww, awh});
  assetWindow();
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
    _rayTracer->setMaxRecursionLevel(_maxRecursionLevel);
    _rayTracer->setMinWeight(_minWeight);
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
