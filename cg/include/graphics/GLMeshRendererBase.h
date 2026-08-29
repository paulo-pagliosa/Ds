//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2020, 2026 Paulo Pagliosa.                        |
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
// OVERVIEW: GLMeshRendererBase.h
// ========
// Class definition for OpenGL mesh renderer base.
//
// Author: Paulo Pagliosa
// Last revision: 29/08/2026

#ifndef __GLMeshRendererBase_h
#define __GLMeshRendererBase_h

#include "graphics/GLGraphics3.h"
#include "graphics/Light.h"
#include "graphics/Material.h"

namespace cg
{ // begin namespace cg


///////////////////////////////////////////////////////////////////////
//
// GLMeshRendererBase: OpenGL mesh renderer base class
// ==================
class GLMeshRendererBase
{
public:
  constexpr static auto maxLights = 8;

  enum class RenderMode
  {
    Wireframe = 1,
    HiddenLines = 2,
    Flat = 4,
    Smooth = 0
  };

  enum class RenderBits
  {
    UseLights = 1,
    UseVertexColors = 2,
    DrawBounds = 4,
    DrawNormals = 8
  };

  using enum RenderMode;
  using enum RenderBits;
  using RenderFlags = Flags<RenderBits>;

  RenderMode renderMode{Smooth};
  RenderFlags flags{UseLights};
  Color boundsColor{255, 102, 0};

  [[nodiscard]] auto useVertexColors() const
  {
    return flags.isSet(UseVertexColors);
  }

}; // GLMeshRendererBase

} // end namespace cg

#endif // __GLMeshRendererBase_h
