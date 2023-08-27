//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2023 Paulo Pagliosa.                        |
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
// OVERVIEW: GLGraphics3.h
// ========
// Class definition for OpenGL 3D graphics.
//
// Author: Paulo Pagliosa
// Last revision: 26/08/2023

#ifndef __GLGraphics3_h
#define __GLGraphics3_h

#include "graphics/Camera.h"
#include "graphics/GLGraphicsBase.h"
#include "graphics/GLMesh.h"
#include <functional>

namespace cg
{ // begin namespace cg

//
// Auxiliary functions
//
inline auto
TRS(const vec3f& p, const mat3f& r, const vec3f& s)
{
  mat4f t{r, p};

  t[0] *= s.x;
  t[1] *= s.y;
  t[2] *= s.z;
  return t;
}

inline auto
normalMatrix(const mat3f& r, const vec3f& s)
{
  auto n = r;

  n[0] *= math::inverse(s.x);
  n[1] *= math::inverse(s.y);
  n[2] *= math::inverse(s.z);
  return n;
}


/////////////////////////////////////////////////////////////////////
//
// GLGraphics3: OpenGL 3D graphics class
// ===========
class GLGraphics3: public GLGraphicsBase, public virtual SharedObject
{
public:
  static TriangleMesh* circle();
  static TriangleMesh* quad();
  static TriangleMesh* box();
  static TriangleMesh* cone();
  static TriangleMesh* sphere();
  static TriangleMesh* cylinder();

  /// Default constructor.
  GLGraphics3();

  /// Returns the color for mesh drawings.
  const auto& meshColor() const
  {
    return _meshColor;
  }

  /// Sets the color for mesh drawings.
  void setMeshColor(const Color& color)
  {
    _meshColor = color;
  }

  /// Returns the color for grid drawing.
  const auto& gridColor() const
  {
    return _gridColor;
  }

  /// Sets the color for grid drawing.
  void setGridColor(const Color& color)
  {
    _gridColor = color;
  }

  /// Returns the color for vector drawings.
  void setVectorColor(const Color& color)
  {
    setLineColor(_meshColor = color);
  }

  /// Draws a mesh.
  void drawMesh(const TriangleMesh&, // mesh
    const mat4f&, // TRS
    const mat3f&); // normal matrix
  void drawMesh(const TriangleMesh&, // mesh
    const vec3f&, // position
    const mat3f&, // rotation
    const vec3f&); // scale

  void drawMesh(const TriangleMesh& mesh)
  {
    drawMesh(mesh, mat4f::identity(), mat3f::identity());
  }

  /// Draws a sub-mesh of a given mesh.
  bool drawSubMesh(const TriangleMesh&, // mesh
    int, // triangle count
    int, // triangle index offset
    const mat4f&, // TRS
    const mat3f&); // normal matrix

  void drawSubMesh(const TriangleMesh& mesh, int count, int offset)
  {
    drawSubMesh(mesh, count, offset, mat4f::identity(), mat3f::identity());
  }

  void drawPoint(const vec3f&);
  void drawLine(const vec3f&, const vec3f&);
  void drawTriangle(const vec3f&, const vec3f&, const vec3f&);
  void drawQuad(const vec3f&, const vec3f&, const vec3f&, const vec3f&);
  void drawCircle(const vec3f&, float, const vec3f&);
  void drawArc(const vec3f&, float, const vec3f&, const vec3f&, float);
  void drawVector(const vec3f&, const vec3f&, float);
  void drawNormals(const TriangleMesh&, const mat4f&, const mat3f&, float = 1);
  void drawAxes(const vec3f&, const mat3f&, float = 1);
  void drawBounds(const Bounds3f&, const mat4f&);
  void drawBounds(const Bounds3f&);
  void drawXZPlane(float, float);

  /// Sets the built-in light position of this object.
  void setCameraLightOffset(const vec3f& offset)
  {
    _lightOffset = offset;
  }

  /// Sets the view-projection matrix of this object.
  void setView(const vec3f& position, const mat4f& vpMatrix)
  {
    _lightPosition = position + _lightOffset;
    _vpMatrix = vpMatrix;
  }

  /// Updates the view-projection matrix of this object.
  void updateView(Camera& camera)
  {
    camera.update();
    setView(camera.position(), vpMatrix(&camera));
  }

  /// Toggles flat mode for mesh drawings.
  void setFlatMode(bool value)
  {
    _flatMode = value;
  }

private:
  using Base = GLGraphicsBase;

  GLSL::Program _meshDrawer;
  mat4f _vpMatrix;
  vec3f _lightOffset;
  vec3f _lightPosition;
  GLint _flatMode;
  GLint _transformLoc;
  GLint _normalMatrixLoc;
  GLint _vpMatrixLoc;
  GLint _lightPositionLoc;
  GLint _colorLoc;
  GLint _flatModeLoc;
  Color _meshColor;
  Color _gridColor;

  void drawPolyline(const vec3f*, int, const mat4f&, bool = false);
  void drawAxis(const vec3f&, const vec3f&, float, TriangleMesh&);

}; // GLGraphics3

} // end namespace cg

#endif // __GLGraphics3_h
