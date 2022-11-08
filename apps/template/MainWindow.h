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
// Class definition for cg template window.
//
// Author: Paulo Pagliosa
// Last revision: 07/11/2022

#ifndef __MainWindow_h
#define __MainWindow_h

#include "graphics/GLRenderWindow3.h"


/////////////////////////////////////////////////////////////////////
//
// MainWindow: template main window class
// ==========
class MainWindow final: public cg::GLRenderWindow3
{
public:
  MainWindow(int width, int height);

private:
  using Base = cg::GLRenderWindow3;

  // Attribute examples
  cg::Color _lineColor;
  cg::Color _meshColor;
  float _radius;
  float _speed;
  bool _animate{true};
  bool _showGround{true};

  // Overridden method examples
  void initialize() override;
  void update() override;
  void renderScene() override;
  bool keyInputEvent(int, int, int) override;
  void gui() override;

}; // MainWindow

#endif // __MainWindow_h
