//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2020, 2023 Paulo Pagliosa.                        |
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
// OVERVIEW: GLRenderWindow3.h
// ========
// Class definition for OpenGL 3D render window.
//
// Author: Paulo Pagliosa
// Last revision: 28/06/2023

#ifndef __GLRenderWindow3_h
#define __GLRenderWindow3_h

#include "core/Flags.h"
#include "graphics/GLGraphics3.h"
#include "graphics/GLWindow.h"

namespace cg
{ // begin namespace cg

template <size_t D> class GLRenderWindow;


/////////////////////////////////////////////////////////////////////
//
// GLRenderWindow3: OpenGL 3D render window
// ===============
template <>
class GLRenderWindow<3>: public GLWindow
{
public:
  using GLWindow::GLWindow;

  GLGraphics3* g3() const
  {
    return _g3;
  }

protected:
  Camera* camera() const
  {
    return _camera;
  }

  void initialize() override;
  void render() override;

  virtual GLGraphics3* makeRenderer(Camera&);
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

  Reference<GLGraphics3> _g3;
  Reference<Camera> _camera;
  Flags<MoveBits> _moveFlags{};
  Flags<DragBits> _dragFlags{};
  int _pivotX;
  int _pivotY;
  int _mouseX;
  int _mouseY;

}; // GLRenderWindow3

using GLRenderWindow3 = GLRenderWindow<3>;

} // end namespace cg

#endif // __GLRenderWindow3_h
