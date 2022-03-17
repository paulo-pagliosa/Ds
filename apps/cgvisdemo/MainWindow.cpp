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
// OVERVIEW: MainWindow.cpp
// ========
// Source file for cg vis demo main window.
//
// Author: Paulo Pagliosa
// Last revision: 16/03/2022

#include "graphics/Application.h"
#include "MainWindow.h"

//
// Demo function
//
extern void demo(cg::vis::Scene&);
extern void demoLog();

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// MainWindow implementation
// ==========
void
MainWindow::beginInitialize()
{
  constexpr auto ffn = "../cgdemo/assets/fonts/Roboto-Regular.ttf";
  auto fonts = ImGui::GetIO().Fonts;

  fonts->AddFontFromFileTTF((Application::baseDirectory() + ffn).c_str(), 16);
}

void
MainWindow::initializeScene()
{
  demo(*scene());
}

inline void
MainWindow::showOptions()
{
  ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.6f);
  ImGui::showStyleSelector("Color Theme##Selector");
  ImGui::colorEdit3("Selected Actor Bounds", _selectedWireframeColor);
  ImGui::PopItemWidth();
}

inline void
MainWindow::fileMenu()
{
  if (ImGui::MenuItem("Exit", "Alt+F4"))
  {
    shutdown();
  }
}

inline void
MainWindow::mainMenu()
{
  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      fileMenu();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("View"))
    {
      ImGui::MenuItem("Scene Window", nullptr, &_showHierarchy);
      ImGui::MenuItem("Inspector Window", nullptr, &_showInspector);
      ImGui::MenuItem("Editor View Settings", nullptr, &_showEditorView);
      ImGui::MenuItem("Demo Code Window", nullptr, &_showLog);
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

inline void
MainWindow::logWindow()
{
  if (!_showLog)
    return;
  if (ImGui::Begin("Demo Code Window",
    nullptr,
    ImGuiWindowFlags_HorizontalScrollbar))
    demoLog();
  ImGui::End();
}

void
MainWindow::gui()
{
  mainMenu();

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
  hierarchyWindow("Scene");
  // Editor View
  ImGui::SetNextWindowPos({lft, evy});
  ImGui::SetNextWindowSize({evw, evh});
  editorView();

  constexpr auto iww = (float)420;
  constexpr auto iwh = (float)380;
  auto rgt = w - iww;
  auto lwy = iwh + top;

  // Inspector Window
  ImGui::SetNextWindowPos({rgt, top});
  ImGui::SetNextWindowSize({iww, iwh});
  inspectorWindow();
  // Log Window
  ImGui::SetNextWindowPos({rgt, lwy});
  ImGui::SetNextWindowSize({iww, h - lwy});
  logWindow();
}

} // end namespace cg::vis
