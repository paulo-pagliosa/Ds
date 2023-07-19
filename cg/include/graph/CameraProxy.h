//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2022, 2023 Paulo Pagliosa.                        |
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
// OVERVIEW: CameraProxy.h
// ========
// Class definition for camera proxy.
//
// Author: Paulo Pagliosa
// Last revision: 19/07/2023

#ifndef __CameraProxy_h
#define __CameraProxy_h

#include "graph/ComponentProxy.h"
#include "graphics/Camera.h"

namespace cg::graph
{ // begin namespace cg::graph


/////////////////////////////////////////////////////////////////////
//
// CameraProxy: camera proxy class
// ===========
class CameraProxy final: public ComponentProxy<Camera>
{
public:
  /// Constructs a default camera.
  static auto New(float aspect = 1)
  {
    return new CameraProxy{aspect};
  }

  static auto New(const Camera& camera)
  {
    return new CameraProxy{camera};
  }

  static Camera* current()
  {
    return _current;
  }

  static void setCurrent(Camera* camera);

  ~CameraProxy() override;

  Camera* camera() const
  {
    return _object;
  }

  void reset(float aspect = 1);

private:
  static Camera* _current;

  CameraProxy(float aspect):
    CameraProxy{*new Camera{aspect}}
  {
    // do nothing
  }

  CameraProxy(const Camera& camera):
    ComponentProxy<Camera>{"Camera", camera}
  {
    // do nothing
  }

  void transformChanged() override;

}; // CameraProxy

inline auto
asCamera(const Component* component)
{
  return dynamic_cast<const CameraProxy*>(component);
}

} // end namespace cg::graph

#endif // __CameraProxy_h
