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
// OVERVIEW: GLPoints3Renderer.h
// ========
// Class definition for OpenGL 3D points renderer.
//
// Author: Paulo Pagliosa
// Last revision: 29/08/2023

#ifndef __GLPoints3Renderer_h
#define __GLPoints3Renderer_h

#include "graphics/CameraHolder.h"
#include "graphics/GLGraphics3.h"
#include "graphics/GLPoints3.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// GLPoints3Renderer: OpenGL 3D points renderer class
// =================
class GLPoints3Renderer: public CameraHolder
{
public:
  bool usePointColors{};

  GLPoints3Renderer(Camera* camera = nullptr):
    CameraHolder{camera}
  {
    // do nothing
  }

  void begin();

  void setPointColor(const Color& color)
  {
    _program.setUniformVec4(_program.pointColorLoc, color);
  }

  void setPointSize(float size)
  {
    _pointSize = size;
  }

  void render(GLPoints3&, const mat4f&);
  void render(GLPoints3&, const vec3f&, const mat3f&, const vec3f&);

  void render(GLPoints3& points)
  {
    render(points, mat4f::identity());
  }

  void end();

private:
  struct GLState
  {
    GLSL::Program* program;
    float pointSize;
    int vao;
  };

  struct GLProgram: public GLSL::Program
  {
    GLint mvpMatrixLoc;
    GLint usePointColorsLoc;
    GLint pointColorLoc;

    GLProgram();

  private:
    void initProgram();
    void initUniformLocations();

  }; // GLProgram

  GLProgram _program;
  GLState _lastState;
  float _pointSize{1};

  void updateView();

}; // GLPoints3Renderer

} // end namespace cg

#endif // __GLPoints3Renderer_h
