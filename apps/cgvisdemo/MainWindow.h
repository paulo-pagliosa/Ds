//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2022 Orthrus Group.                         |
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
// Class definition for cg vis demo main window.
//
// Author: Paulo Pagliosa
// Last revision: 16/03/2022

#ifndef __MainWindow_h
#define __MainWindow_h

#include "SceneWindow.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// MainWindow: cg vis demo main window class
// ==========
class MainWindow final: public SceneWindow
{
public:
  MainWindow(int width, int height):
    SceneWindow{"Ds-Vis Demo Version 1.0", width, height}
  {
    // do nothing
  }

private:
  bool _showLog{true};

  void beginInitialize() override;
  void initializeScene() override;
  void gui() override;

  void mainMenu();
  void fileMenu();
  void showOptions();
  void logWindow();

}; // MainWindow

} // end namespace cg::vis

#endif // __MainWindow_h
