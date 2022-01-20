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
// OVERVIEW: GLRenderer.h
// ========
// Class definition for OpenGL Renderer.
//
// Author: Paulo Pagliosa
// Last revision: 20/01/2022

#ifndef __GLRenderer_h
#define __GLRenderer_h

#include "graphics/GLGraphics3.h"
#include "graphics/GLRendererBase.h"

namespace cg
{ // begin namespace Graphics

#define MAX_LIGHTS 8


//////////////////////////////////////////////////////////
//
// GLRenderer: OpenGL renderer class
// ==========
class GLRenderer: public GLRendererBase, public GLGraphics3
{
public:
  using RenderFunction = void (*)(GLRenderer&);
  using GLGraphics3::drawMesh;

  /// Constructs a GL renderer object.
  GLRenderer(SceneBase& scene, Camera& camera);

  /// Destructor.
  ~GLRenderer();

  void update();
  void render();

  using GLGraphics3::drawAxes;

  bool drawMesh(const Primitive& primitive) sealed;
  void renderMaterial(const Material& material) sealed;

  void setRenderFunction(RenderFunction f)
  {
    _renderFunction = f;
  }

protected:
  RenderFunction _renderFunction{};

  virtual void beginRender();
  virtual void endRender();
  virtual void renderActors();
  virtual void renderLights();

  void drawAxes(const mat4f&);

private:
  struct GLData;

  GLData* _gl;

}; // GLRenderer

} // end namespace cg

#endif // __GLRenderer_h
