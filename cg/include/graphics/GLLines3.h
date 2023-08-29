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
// OVERVIEW: GLLines3.h
// ========
// Class definition for OpenGL 3D line buffer object.
//
// Author: Paulo Pagliosa
// Last revision: 29/08/2023

#ifndef __GLLines3_h
#define __GLLines3_h

#include "geometry/Index2.h"
#include "graphics/GLPoints3.h"
#include <cassert>

namespace cg
{ // begin namespace cg

class GLLines3Renderer;


/////////////////////////////////////////////////////////////////////
//
// GLLines3: OpenGL 3D line buffer object class
// ========
class GLLines3: public GLPoints3
{
public:
  using IndexArray = std::vector<uint32_t>;

  GLLines3(const PointArray& points, IndexArray&& lineSizes);

  GLLines3(const PointArray& points, uint32_t lineCount, uint32_t lineSize):
    GLLines3{points, IndexArray(lineCount, lineSize)}
  {
    // do nothing
  }

  auto lineCount() const
  {
    return uint32_t(_lineEnds.size());
  }

private:
  IndexArray _lineEnds;

  /// Returns the point indices (start,end+1) of the ith line.
  auto lineIndices(uint32_t i) const
  {
    assert(i < _lineEnds.size());
    return Index2<uint32_t>{i ? _lineEnds[i - 1] : 0, _lineEnds[i]};
  }

  friend GLLines3Renderer;
  friend GLRenderer;

}; // GLLines3

} // end namespace cg

#endif // __GLLines3_h
