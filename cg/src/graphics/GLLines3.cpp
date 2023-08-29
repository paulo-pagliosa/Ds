//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2023 Paulo Pagliosa.                              |
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
// OVERVIEW: GLLines3.cpp
// ========
// Source file for OpenGL 3D line buffer object.
//
// Author: Paulo Pagliosa
// Last revision: 29/08/2023

#include "graphics/GLLines3.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// GLLines3 implementation
// ========
GLLines3::GLLines3(const PointArray& points, IndexArray&& lineSizes):
  GLPoints3{points},
  _lineEnds(std::move(lineSizes))
{
  for (size_t n = _lineEnds.size(), i = 1; i < n; ++i)
    _lineEnds[i] += _lineEnds[i - 1];
}

} // end namespace cg
