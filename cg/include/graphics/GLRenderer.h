//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2026 Paulo Pagliosa.                        |
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
// OVERVIEW: GLRenderer.h
// ========
// Class definition for OpenGL Renderer.
//
// Author: Paulo Pagliosa
// Last revision: 01/09/2026

#ifndef __GLRenderer_h
#define __GLRenderer_h

#include "graphics/GLGraphics3.h"
#include "graphics/GLMeshRendererBase.h"
#include "graphics/Renderer.h"
#include <functional>

namespace cg
{ // begin namespace Graphics


/////////////////////////////////////////////////////////////////////
//
// GLRenderer: OpenGL renderer class
// ==========
class GLRenderer: public Renderer, public GLMeshRendererBase  
{
public:
  using RenderFunction = std::function<void(GLRenderer&)>;

  using GLGraphics3::drawAxes;
  using GLGraphics3::drawMesh;
  using GLGraphics3::drawSubMesh;

  /// Constructs a GL renderer object.
  GLRenderer(SceneBase& scene, Camera& camera);

  void setImageSize(int w, int h)
  {
    Renderer::setImageSize(w, h);
    glViewport(0, 0, w, h);
  }

  void update()
  {
    updateView(*camera());
  }

  void render() override;

  bool drawMesh(const Primitive& primitive);

  bool drawSubMesh(const TriangleMesh& mesh,
    int count,
    int index,
    const Material& material,
    const mat4f& t,
    const mat3f& n);

  void setRenderFunction(RenderFunction f)
  {
    _renderFunction = f;
  }

  void setBasePoint(const vec3f& p);

  [[nodiscard]] float pixelsLength(float d) const;

protected:
  RenderFunction _renderFunction;
  vec3f _basePoint;
  float _basePointZ;
  float _windowViewportRatio;

  void updateView(Camera&) override;

  virtual void beginRender();
  virtual void endRender();
  virtual void renderActors();
  virtual void renderLights();

  void drawAxes(const mat4f&, float);

}; // GLRenderer

} // end namespace cg

#endif // __GLRenderer_h
