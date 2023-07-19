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
// OVERVIEW: LightProxy
// ========
// Class definition for light proxy.
//
// Author: Paulo Pagliosa
// Last revision: 19/07/2023

#ifndef __LightProxy_h
#define __LightProxy_h

#include "graph/ComponentProxy.h"
#include "graphics/Light.h"

namespace cg::graph
{ // begin namespace cg::graph


/////////////////////////////////////////////////////////////////////
//
// LightProxy: light proxy class
// ==========
class LightProxy final: public ComponentProxy<Light>
{
public:
  /// Constructs a default light.
  static auto New()
  {
    return new LightProxy;
  }

  static auto New(const Light& light)
  {
    return new LightProxy{light};
  }

  Light* light() const
  {
    return _object;
  }

private:
  LightProxy():
    LightProxy{*new Light}
  {
    // do nothing
  }

  LightProxy(const Light& light):
    ComponentProxy<Light>{"Light", light}
  {
    // do nothing
  }

  void afterAdded() override;
  void beforeRemoved() override;
  void transformChanged() override;
  void setVisible(bool value) override;

}; // LightProxy

inline auto
asLight(const Component* component)
{
  return dynamic_cast<const LightProxy*>(component);
}

} // end namepace cg::graph

#endif // __LightProxy_h
