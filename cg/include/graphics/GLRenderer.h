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
// OVERVIEW: GLRenderer.h
// ========
// Class definition for OpenGL Renderer.
//
// Author: Paulo Pagliosa
// Last revision: 02/09/2023

#ifndef __GLRenderer_h
#define __GLRenderer_h

#include "graphics/GLGraphics3.h"
#include "graphics/GLRendererBase.h"
#include <functional>

namespace cg
{ // begin namespace Graphics


//////////////////////////////////////////////////////////
//
// GLRenderer: OpenGL renderer class
// ==========
class GLRenderer: public GLRendererBase, public GLGraphics3
{
public:
  constexpr static auto maxLights = 8;

  using RenderFunction = std::function<void(GLRenderer&)>;
  using enum RenderMode;

  using GLGraphics3::drawAxes;
  using GLGraphics3::drawMesh;
  using GLGraphics3::drawSubMesh;

  /// Constructs a GL renderer object.
  GLRenderer(SceneBase& scene, Camera& camera);

  /// Destructor.
  ~GLRenderer();

  void update();
  void render();

  void renderMaterial(const Material& material) final;
  bool drawMesh(const Primitive& primitive) final;

  bool drawSubMesh(const TriangleMesh& mesh,
    int count,
    int offset,
    const Material& material,
    const mat4f& t,
    const mat3f& n);

  void setRenderFunction(RenderFunction f)
  {
    _renderFunction = f;
  }

  void setBasePoint(const vec3f& p);

  float pixelsLength(float d) const;

protected:
  RenderFunction _renderFunction;
  vec3f _basePoint;
  float _basePointZ;
  float _windowViewportRatio;

  virtual void beginRender();
  virtual void endRender();
  virtual void renderActors();
  virtual void renderLights();

  void drawAxes(const mat4f&, float);
  void drawMesh(const TriangleMesh&,
    const Material&,
    const mat4f&,
    const mat3f&,
    int,
    int);

private:
  struct GLData;

  GLData* _gl;

}; // GLRenderer

} // end namespace cg

#endif // __GLRenderer_h
