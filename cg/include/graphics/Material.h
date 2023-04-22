//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2023 Paulo Pagliosa.                        |
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
// OVERVIEW: Material.h
// ========
// Class definition for material.
//
// Author: Paulo Pagliosa
// Last revision: 21/04/2023

#ifndef __Material_h
#define __Material_h

#include "core/NameableObject.h"
#include "graphics/Color.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Material: material class
// ========
class Material: public NameableObject
{
public:
  Color ambient; // ambient color
  Color diffuse; // diffuse color
  Color spot; // specular spot color
  float shine; // specular spot exponent
  Color specular; // specular color
  Color transparency; // transparency color
  float ior; // index of refraction

  static const Material* defaultMaterial();

  Material(const Color& color):
    ambient{0.2f * color},
    diffuse{0.8f * color},
    shine{100},
    transparency{Color::black},
    ior{1}
  {
    spot = specular = Color::white;
  }

  auto clone() const
  {
    return new Material{*this};
  }

private:
  Material():
    Material{Color::white}
  {
    setName("Default");
  }

}; // Material

inline const Material*
Material::defaultMaterial()
{
  static const Material* dm{};

  if (dm == nullptr)
    dm = Material::makeUse(new Material);
  return dm;
}

} // end namespace cg

#endif // __Material_h
