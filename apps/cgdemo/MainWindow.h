//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2022 Paulo Pagliosa.                              |
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
// OVERVIEW: MainWindow.h
// ========
// Class definition for cg demo main window.
//
// Author: Paulo Pagliosa
// Last revision: 22/07/2022

#ifndef __MainWindow_h
#define __MainWindow_h

#include "graph/SceneWindow.h"
#include "graphics/Assets.h"
#include "graphics/GLImage.h"
#include "RayTracer.h"

using namespace cg;
using namespace cg::graph;


/////////////////////////////////////////////////////////////////////
//
// MainWindow: cg demo main window class
// ==========
class MainWindow final: public SceneWindow
{
public:
  MainWindow(int width, int height):
    SceneWindow{"Ds Demo Version 1.1", width, height}
  {
    // do nothing
  }

private:
  Reference<RayTracer> _rayTracer;
  Reference<GLImage> _image;
  int _maxRecursionLevel{6};
  float _minWeight{RayTracer::minMinWeight};

  static MeshMap _defaultMeshes;

  auto makeDefaultPrimitive(const char* const meshName)
  {
    return makePrimitive(*_defaultMeshes[meshName], meshName);
  }

  auto createDefaultPrimitiveObject(const char* const meshName)
  {
    return createPrimitiveObject(*_defaultMeshes[meshName], meshName);
  }

  void beginInitialize() override;
  void initializeScene() override;
  void renderScene() override;
  Component* addComponentMenu() override;
  void createObjectMenu() override;
  bool onResize(int width, int height) override;
  void gui() override;

  void mainMenu();
  void fileMenu();
  void createMenu();
  void showOptions();

  void readScene(const std::string& filename);

  static void buildDefaultMeshes();

}; // MainWindow

#endif // __MainWindow_h
