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
// OVERVIEW: GLRenderWindow2.h
// ========
// Class definition for OpenGL 2D render window.
//
// Author: Paulo Pagliosa
// Last revision: 28/06/2023

#ifndef __GLRenderWindow2_h
#define __GLRenderWindow2_h

#include "core/Flags.h"
#include "graphics/GLGraphics2.h"
#include "graphics/GLWindow.h"

namespace cg
{ // begin namespace cg

template <size_t D> class GLRenderWindow;


/////////////////////////////////////////////////////////////////////
//
// GLRenderWindow2: OpenGL 2D render window
// ===============
template <>
class GLRenderWindow<2>: public GLWindow
{
public:
  using GLWindow::GLWindow;

  GLGraphics2* g2() const
  {
    return _g2;
  }

protected:
  void initialize() override;
  void render() override;

  virtual void renderScene();

  bool windowResizeEvent(int, int) override;
  bool keyInputEvent(int, int, int) override;
  bool scrollEvent(double, double) override;
  bool mouseButtonInputEvent(int, int, int) override;
  bool mouseMoveEvent(double, double) override;

  virtual bool onMouseLeftPress(int, int);

private:
  enum class MoveBits
  {
    Left = 1,
    Right = 2,
    Up = 4,
    Down = 8
  };

  enum class DragBits
  {
    Rotate = 1,
    Pan = 2
  };

  Reference<GLGraphics2> _g2;
  Flags<MoveBits> _moveFlags{};
  Flags<DragBits> _dragFlags{};
  int _pivotX;
  int _pivotY;
  int _mouseX;
  int _mouseY;

}; // GLRenderWindow2

using GLRenderWindow2 = GLRenderWindow<2>;

} // end namespace cg

#endif // __GLRenderWindow2_h
