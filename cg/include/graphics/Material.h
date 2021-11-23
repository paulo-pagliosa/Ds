//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2019 Orthrus Group.                         |
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
// Last revision: 27/05/2019

#ifndef __Material_h
#define __Material_h

#include "graphics/Image.h"

namespace cg
{ // begin namespace cg

using TextureId = void*;


/////////////////////////////////////////////////////////////////////
//
// Material: material class
// ========
class Material
{
public:
  Color ambient; // ambient color
  Color diffuse; // diffuse color
  Color spot; // specular spot color
  float shine; // specular spot exponent
  TextureId texture{}; // TODO

  Material(const Color& color = Color::white):
    ambient{0.2f * color},
    diffuse{0.8f * color},
    spot{Color::black},
    shine{100}
  {
    // do nothing
  }

}; // Material

} // end namespace cg

#endif // __Material_h
