//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2022 Paulo Pagliosa.                              |
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
// OVERVIEW: LightProxy.h
// ========
// Class definition for vis light proxy.
//
// Author: Paulo Pagliosa
// Last revision: 28/02/2022

#ifndef __VisLightProxy_h
#define __VisLightProxy_h

#include "graphics/Light.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// LightProxy: vis light proxy class
// ==========
class LightProxy: public Light
{
public:

  const auto& direction() const
  {
    return _eulerAngles;
  }

  void setDirection(const vec3f& angles)
  {
    _rotation = quatf::eulerAngles(_eulerAngles = angles);
    Light::direction = _rotation * vec3f{0, 0, 1};
  }

private:
  vec3f _eulerAngles{vec3f::null()};
  quatf _rotation{quatf::identity()};

}; // LightProxy

inline auto
asLightProxy(Light* light)
{
  return dynamic_cast<LightProxy*>(light);
}

} // end namespace cg::vis

#endif // __VisLightProxy_h