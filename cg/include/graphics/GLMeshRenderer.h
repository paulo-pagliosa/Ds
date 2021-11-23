//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019, 2020 Orthrus Group.                         |
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
// Last revision: 17/08/2020

#ifndef __GLMeshRenderer_h
#define __GLMeshRenderer_h

#include "graphics/GLGraphics3.h"
#include "graphics/Light.h"
#include "graphics/Material.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// GLMeshRenderer: OpenGL mesh renderer class
// ==============
class GLMeshRenderer: public SharedObject
{
public:
  constexpr static auto maxLights = 8;

  enum RenderMode
  {
    Wireframe = 1,
    HiddenLines = 2,
    Flat = 4,
    Smooth = 0
  };

  enum RenderBits
  {
    UseLights = 1,
    UserVertexColors = 2
  };

  using RenderFlags = Flags<RenderBits>;

  RenderMode renderMode{RenderMode::Smooth};
  RenderFlags flags{RenderBits::UseLights};

  // Constructor.
  GLMeshRenderer(Camera* camera = nullptr);

  Camera* camera() const
  {
    return _camera;
  }

  auto useVertexColors() const
  {
    return flags.isSet(RenderBits::UserVertexColors);
  }

  void setCamera(Camera* camera);

  void begin();

  void setAmbientLight(const Color& color)
  {
    _program.setUniformVec4(_ambientLightLoc, color);
  }

  template <typename LightIterator>
  void setLights(LightIterator begin, LightIterator end);

  void setLineColor(const Color& color)
  {
    _program.setUniformVec4(_lineColorLoc, color);
  }

  void setMaterial(const Material& material);

  void render(TriangleMesh&, const mat4f&, const mat3f&);
  void render(TriangleMesh&, const vec3f&, const mat3f&, const vec3f&);

  void render(TriangleMesh& mesh)
  {
    render(mesh, mat4f::identity(), mat3f::identity());
  }

  void end();

private:
  struct GLState
  {
    GLSL::Program* program;
    bool depthTest;
    int texture;
    int vao;
  };

  struct LightPropertyLoc
  {
    GLint position;
    GLint color;
  };

  Reference<Camera> _camera;
  GLSL::Program _program;
  mat4f _viewportMatrix;
  int _lightCount{};
  GLuint _texture{};
  GLint _mvMatrixLoc;
  GLint _normalMatrixLoc;
  GLint _mvpMatrixLoc;
  GLint _viewportMatrixLoc;
  GLint _ambientLightLoc;
  GLint _lightCountLoc;
  LightPropertyLoc _lightLocs[maxLights];
  GLint _lightTypeLocs[maxLights];
  GLint _useTextureLoc;
  GLint _OaLoc;
  GLint _OdLoc;
  GLint _OsLoc;
  GLint _nsLoc;
  GLint _lineWidthLoc;
  GLint _lineColorLoc;
  GLuint _noMixIdx;
  GLuint _lineColorMixIdx;
  GLuint _modelMaterialIdx;
  GLuint _colorMapMaterialIdx;
  GLState _lastState;

  void initProgram();
  void initUniformLocations();
  void initSubroutineIndices();

  void setDefaultLights();
  bool setLight(int, const Light&);

  void updateView();

}; // GLMeshRenderer

template <typename LightIterator>
void
GLMeshRenderer::setLights(LightIterator begin, LightIterator end)
{
  _lightCount = 0;
  while (begin != end)
    if (setLight(_lightCount, *begin++))
      if (++_lightCount == maxLights)
        break;
  _program.setUniform(_lightCountLoc, _lightCount);
}

} // end namespace cg

#endif // __GLMeshRenderer_h
