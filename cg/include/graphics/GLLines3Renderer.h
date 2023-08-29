//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2023 Paulo Pagliosa.                              |
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
// OVERVIEW: GLLines3Renderer.h
// ========
// Class definition for OpenGL 3D lines renderer.
//
// Author: Paulo Pagliosa
// Last revision: 29/08/2023

#ifndef __GLLines3Renderer_h
#define __GLLines3Renderer_h

#include "graphics/CameraHolder.h"
#include "graphics/GLGraphics3.h"
#include "graphics/GLLines3.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// GLLines3Renderer: OpenGL 3D lines renderer class
// ================
class GLLines3Renderer: public CameraHolder
{
public:
  bool usePointColors{};

  GLLines3Renderer(Camera* camera = nullptr):
    CameraHolder{camera}
  {
    // do nothing
  }

  void begin();

  void setLineColor(const Color& color)
  {
    _program.setUniformVec4(_program.lineColorLoc, color);
  }

  void setLineWidth(float width)
  {
    _lineWidth = width;
  }

  void render(GLLines3&, const mat4f&);
  void render(GLLines3&, const vec3f&, const mat3f&, const vec3f&);

  void render(GLLines3& lines)
  {
    render(lines, mat4f::identity());
  }

  void end();

private:
  struct GLState
  {
    GLSL::Program* program;
    float lineWidth;
    int vao;
  };

  struct GLProgram: public GLSL::Program
  {
    GLint mvpMatrixLoc;
    GLint usePointColorsLoc;
    GLint lineColorLoc;

    GLProgram();

  private:
    void initProgram();
    void initUniformLocations();

  }; // GLProgram

  GLProgram _program;
  GLState _lastState;
  float _lineWidth{1};

  void updateView();

}; // GLLines3Renderer

} // end namespace cg

#endif // __GLLines3Renderer_h
