//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019, 2023 Paulo Pagliosa.                        |
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
// Last revision: 29/08/2023

#ifndef __GLMeshRenderer_h
#define __GLMeshRenderer_h

#include "graphics/CameraHolder.h"
#include "graphics/GLGraphics3.h"
#include "graphics/Light.h"
#include "graphics/Material.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// GLMeshRenderer: OpenGL mesh renderer class
// ==============
class GLMeshRenderer: public CameraHolder
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
    UseVertexColors = 2
  };

  using enum RenderMode;
  using enum RenderBits;
  using RenderFlags = Flags<RenderBits>;

  RenderMode renderMode{Smooth};
  RenderFlags flags{UseLights};

  GLMeshRenderer(Camera* camera = nullptr):
    CameraHolder{camera}
  {
    // do nothing
  }

  auto useVertexColors() const
  {
    return flags.isSet(UseVertexColors);
  }

  void begin();

  void setAmbientLight(const Color& color)
  {
    _program.setUniformVec4(_program.ambientLightLoc, color);
  }

  template <typename LightIterator>
  void setLights(LightIterator, LightIterator);

  void setLineColor(const Color& color)
  {
    _program.setUniformVec4(_program.lineColorLoc, color);
  }

  void setMaterial(const Material&, void* = nullptr);

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

  struct GLProgram: public GLSL::Program
  {
    struct LightPropLoc
    {
      GLint type;
      GLint color;
      GLint position;
      GLint direction;
      GLint falloff;
      GLint range;
      GLint angle;
    };

    GLint mvMatrixLoc;
    GLint normalMatrixLoc;
    GLint mvpMatrixLoc;
    GLint viewportMatrixLoc;
    GLint projectionTypeLoc;
    GLint ambientLightLoc;
    GLint useTextureLoc;
    GLint lightCountLoc;
    LightPropLoc lightLocs[maxLights];
    GLint OaLoc;
    GLint OdLoc;
    GLint OsLoc;
    GLint nsLoc;
    GLint lineWidthLoc;
    GLint lineColorLoc;
    GLuint noMixIdx;
    GLuint lineColorMixIdx;
    GLuint modelMaterialIdx;
    GLuint colorMapMaterialIdx;

    GLProgram();

    void renderMaterial(const Material&);
    void renderLight(int, const Light&, const Camera&);
    void renderDefaultLights();

  private:
    GLint uniformLightLocation(int i, const char* field)
    {
      constexpr auto maxName = 32;
      char name[maxName];

      snprintf(name, maxName, "lights[%d].%s", i, field);
      return uniformLocation(name);
    }

    void initProgram();
    void initUniformLocations();
    void initSubroutineIndices();
    void initUniformLightLocations(int);

  }; // GLProgram

  GLProgram _program;
  GLState _lastState;
  mat4f _viewportMatrix;
  int _lightCount{};
  GLuint _texture{};

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
  _program.setUniform(_program.lightCountLoc, _lightCount);
}

} // end namespace cg

#endif // __GLMeshRenderer_h
