//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019, 2026 Paulo Pagliosa.                        |
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
// OVERVIEW: GLMeshRenderer.h
// ========
// Class definition for OpenGL mesh renderer.
//
// Author: Paulo Pagliosa
// Last revision: 01/09/2026

#ifndef __GLMeshRenderer_h
#define __GLMeshRenderer_h

#include "graphics/CameraHolder.h"
#include "graphics/GLMeshRendererBase.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// GLMeshRenderer: OpenGL mesh renderer class
// ==============
class GLMeshRenderer: public GLMeshRendererBase, public CameraHolder
{
public:
  GLMeshRenderer(Camera* camera = nullptr):
    CameraHolder{camera}
  {
    // do nothing
  }

  void begin()
  {
    GLMeshRendererBase::begin(*camera());
  }

  template <typename LightIt>
  void setLights(LightIt begin, LightIt end)
  {
    GLMeshRendererBase::setLights(begin, end, *camera());
  }

  void render(const TriangleMesh& mesh, // mesh
    const mat4f& t, // local to world matriz
    const mat3f& n) // normal matrix
  {
    GLMeshRendererBase::render(mesh, t, n, *camera());
  }

  void render(const TriangleMesh& mesh)
  {
    render(mesh, mat4f::identity(), mat3f::identity());
  }

  void render(const TriangleMesh&, // mesh
    const vec3f&, // position
    const mat3f&, // rotation
    const vec3f&); // scale

}; // GLMeshRenderer

} // end namespace cg

#endif // __GLMeshRenderer_h
