//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019, 2022 Paulo Pagliosa.                        |
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
// OVERVIEW: SceneEditor.h
// ========
// Class definition for scene editor.
//
// Author: Paulo Pagliosa
// Last revision: 26/02/2022

#ifndef __SceneEditor_h
#define __SceneEditor_h

#include "graphics/GLRenderer.h"

namespace cg
{ // begin namespace cg

class SceneWindowBase;


/////////////////////////////////////////////////////////////////////
//
// SceneEditor: scene editor class
// ===========
class SceneEditor: public GLRenderer
{
public:
  bool showGround{true};

  void setDefaultView(float aspect = 1);
  void zoom(float s);
  void rotateView(float ax, float ay);
  void orbit(float ax, float ay);
  void pan(const vec3f& d);

  void pan(float dx, float dy, float dz)
  {
    pan({dx, dy, dz});
  }

  auto orbitDistance() const
  {
    return _orbitDistance;
  }

  void newFrame();
  void drawGround();

  void drawCamera(const Camera&);
  void drawLight(const Light&);
  void drawTransform(const vec3f&, const quatf&);

private:
  float _orbitDistance{10};
  Color _lightGismoColor{255, 255, 153};
  Color _cameraLineColor{255, 255, 153};

  SceneEditor(SceneBase& scene):
    GLRenderer{scene, *new Camera}
  {
    // do nothing
  }

  void outlineSphere(const vec3f&, float, const Color&);
  void outlineCone(const vec3f&, const mat3f&, float, float);
  void outlineCylinder(const vec3f&, const mat3f&, float, float);

  friend SceneWindowBase;

}; // SceneEditor

} // end namespace cg

#endif // __SceneEditor_h
