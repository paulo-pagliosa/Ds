//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2016, 2026 Paulo Pagliosa.                        |
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
// OVERVIEW: GLParticleRenderer.h
// ========
// Class definition for OpenGL particle renderer.
//
// Author: Paulo Pagliosa
// Last revision: 28/10/2026

#ifndef __GLParticleRenderer_h
#define __GLParticleRenderer_h

#include "graphics/CameraHolder.h"
#include "graphics/GLPoints3.h"
#include "graphics/GLProgram.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// GLParticleRenderer: OpenGL particle renderer class
// ==================
class GLParticleRenderer: public SharedCameraHolder
{
public:
  bool useParticleColors{};

  GLParticleRenderer(Camera* camera = nullptr):
    SharedCameraHolder{camera}
  {
    // do nothing
  }

  void begin();
  void setParticleColor(const Color& color);

  void setRadius(float radius)
  {
    if (radius >= minRadius)
      _radius = radius;
  }

  void render(const GLPoints3& points, const mat4f& t)
  {
    drawParticles(points, t, (int)points.size(), 0);
  }

  void render(const GLPoints3& points)
  {
    render(points, mat4f::identity());
  }

  template <IsVec3Array PA>
  void render(const PA& points, const mat4f& t)
  {
    Reference<GLPoints3> temp{new GLPoints3{points}};
    render(temp, t);
  }

  void end();

private:
  constexpr static auto minRadius = 0.001f;

  struct GLState
  {
    GLSL::Program* program;
    int vao;
  };

  struct GLProgram: public GLSL::Program
  {
    GLint mvMatrixLoc;
    GLint projectionMatrixLoc;
    GLint viewportSizeLoc;
    GLint useParticleColorsLoc;
    GLint particleColorLoc;
    GLint radiusLoc;

    GLProgram();

  private:
    void initProgram();
    void initUniformLocations();

    void setParticleColor(const Color&);
    void setMVP(const mat4f&, const Camera&);
    void setViewportSize(float, float);
    void setUseParticleColors(bool);
    void setRadius(float);

    friend GLParticleRenderer;

  }; // GLProgram

  GLProgram _program;
  GLState _lastState;
  float _radius{minRadius};

  void updateView();
  void drawParticles(const GLPoints3&, const mat4f&, int, int);

}; // GLParticleRenderer

} // end namespace cg

#endif // __GLParticleRenderer_h
