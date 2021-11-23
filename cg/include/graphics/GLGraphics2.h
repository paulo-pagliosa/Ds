//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019 Orthrus Group.                               |
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
// OVERVIEW: GLGraphics2.h
// ========
// Class definition for OpenGL 2D graphics.
//
// Author: Paulo Pagliosa
// Last revision: 16/02/2019

#ifndef __GLGraphics2_h
#define __GLGraphics2_h

#include "core/SharedObject.h"
#include "geometry/Bounds2.h"
#include "graphics/GLGraphicsBase.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// View2: 2D view class
// =====
class View2: public SharedObject
{
public:
  /// Default constructor.
  View2();

  /// Returns the bounds of this view.
  const Bounds2f& bounds() const
  {
    return _bounds;
  }

  /// Sets the bounds of this view.
  void setBounds(const Bounds2f& bounds);

  /// Returns the aspect ratio of this view.
  float aspectRatio() const
  {
    return _aspectRatio;
  }

  /// Sets the aspect ratio of this view.
  void setAspectRatio(float aspectRatio);

  /// Changes the window size ot this view.
  void zoom(float scale);

  /// Changes the window center of this view.
  void pan(float x, float y);

  /// Updates the view matrix of this view.
  void updateView();

  /// Sets the default view of this view.
  void setDefaultView();

protected:
  Bounds2f _bounds;
  float _aspectRatio;
  vec2f _delta;
  float _scale;
  float _sx;
  float _sy;
  float _tx;
  float _ty;
  bool _modified;

  void toNDC(const vec2f& p, vec4f& v) const;

}; // View2


/////////////////////////////////////////////////////////////////////
//
// GLGraphics2: OpenGL 2D graphics class
// ===========
class GLGraphics2: public GLGraphicsBase, public View2
{
public:
  void drawPoint(const vec2f& p);
  void drawLine(const vec2f& p1, const vec2f& p2);
  void drawTriangle(const vec2f& p1, const vec2f& p2, const vec2f& p3);
  void drawCircumference(const vec2f& center, float radius);
  void drawBounds(const Bounds2f& bounds);

  /// Transform screen coordinates \c p to world coordinates.
  vec2f screenToWorld(const vec2f& p) const;

private:
  using Base = GLGraphicsBase;

}; // GLGraphics2

} // end namespace cg

#endif // __GLGraphics2_h
