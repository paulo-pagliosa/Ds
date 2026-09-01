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
// Last revision: 31/08/2026

#ifndef __GLMeshRendererBase_h
#define __GLMeshRendererBase_h

#include "graphics/GLGraphics3.h"
#include "graphics/Light.h"
#include "graphics/Material.h"

namespace cg
{ // begin namespace cg

using TextureId = void*;


///////////////////////////////////////////////////////////////////////
//
// GLMeshRendererBase: OpenGL mesh renderer base class
// ==================
class GLMeshRendererBase: public GLGraphics3
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

  /// Destructor.
  ~GLMeshRendererBase() override;

  [[nodiscard]] auto useVertexColors() const
  {
    return flags.isSet(UseVertexColors);
  }

  void setLineColor(const Color&);
  void setAmbientLight(const Color&);
  void setMaterial(const Material&, TextureId = {});
  void end();

protected:
  /// Constructs a GLMeshRendererBase.
  GLMeshRendererBase();

  template <typename LightIt>
  void setLights(LightIt, LightIt, const Camera&);

  void updateView(Camera&);
  void begin(Camera&);

  void render(TriangleMesh&, // mesh
    const mat4f&, // local to global matrix
    const mat3f&, // normal matrix
    const Camera&); // camera

private:
  struct GLState
  {
    GLSL::Program* program;
    bool depthTest;
    int texture;
    int vao;

  }; // GLState

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
    LightPropLoc lightLocs[maxLights];
    GLint lightCountLoc;
    GLint OaLoc;
    GLint OdLoc;
    GLint OsLoc;
    GLint nsLoc;
    GLint lineWidthLoc;
    GLint lineColorLoc;
    GLint hiddenLinesFlagLoc;
    GLint useTextureLoc;
    GLint useVertexColorsLoc;

  private:
    GLProgram(GLMeshRendererBase&);

    GLint uniformLightLocation(int, const char*);
    void initUniformLightLocations(int);
    void initUniformLocations();
    void initProgram();

    void setLineColor(const Color&);
    void setAmbientLight(const Color&);
    void setMaterial(const Material&);
    void setTransforms(const mat4f&, const mat3f&, const Camera&);
    bool setLight(int, const Light&, const Camera&);
    void setDefaultLights();
    void setHiddenLinesFlag(bool);
    void setUseTexture(bool);
    void setUseVertexColors(bool);

    friend GLMeshRendererBase;

  }; // GLProgram

  GLProgram* _program{};
  GLState _lastState;
  mat4f _viewportMatrix;
  int _lightCount{};
  GLuint _texture{};

}; // GLMeshRendererBase

template <typename LightIt>
void
GLMeshRendererBase::setLights(LightIt begin, LightIt end, const Camera& camera)
{
  _program->assertInUse();
  _lightCount = 0;
  while (begin != end)
    if (_program->setLight(_lightCount, *begin++, camera))
      if (++_lightCount == maxLights)
        break;
  _program->setUniform(_program->lightCountLoc, _lightCount);
}

} // end namespace cg

#endif // __GLMeshRendererBase_h
