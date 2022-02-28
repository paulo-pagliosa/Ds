//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2020, 2022 Paulo Pagliosa.                        |
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
// OVERVIEW: GLRendererBase.h
// ========
// Class definition for OpenGL renderer base.
//
// Author: Paulo Pagliosa
// Last revision: 28/02/2022

#ifndef __GLRendererBase_h
#define __GLRendererBase_h

#include "core/Flags.h"
#include "geometry/TriangleMesh.h"
#include "graphics/Renderer.h"
#include "graphics/Material.h"

namespace cg
{ // begin namespace cg


//////////////////////////////////////////////////////////
//
// GLRendererBase: OpenGL renderer base class
// ==============
class GLRendererBase: public Renderer
{
public:
  enum class RenderMode
  {
    Wireframe = 1,
    HiddenLines = 2,
    Flat = 4,
    Smooth = 0
  };

  enum RenderBits
  {
    UseLights = 1,
    UseVertexColors = 2,
    DrawBounds = 4,
    DrawNormals = 8
  };

  using RenderFlags = Flags<RenderBits>;

  RenderMode renderMode;
  RenderFlags flags;
  Color boundsColor;

  using Renderer::Renderer;

  virtual bool drawMesh(const Primitive& primitive) = 0;
  virtual void renderMaterial(const Material& material) = 0;

protected:
  /// Constructs a GL renderer object.
  GLRendererBase(SceneBase& scene, Camera& camera);

}; // GLRendererBase

} // end namespace cg

#endif // __GLRendererBase_h
