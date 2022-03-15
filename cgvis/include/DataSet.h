//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2022 Paulo Pagliosa.                        |
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
// OVERVIEW: DataSet.h
// ========
// Class definition for generic vis data set.
//
// Author: Paulo Pagliosa
// Last revision: 14/03/2022

#ifndef __DataSet_h
#define __DataSet_h

#include "math/Vector3.h"
#include "Scalars.h"

namespace cg::vis
{ // begin namespace cg::vis

using Vectors = DataArray<vec3f>;


/////////////////////////////////////////////////////////////////////
//
// DataSet: generic vis data set class
// =======
class DataSet: public virtual Object
{
public:
  int32_t vertexCount() const
  {
    return 0;
  }

  Scalars* vertexScalars() const
  {
    return _vertexScalars;
  }

  void setVertexScalars(const Scalars* scalars)
  {
    if (scalars != _vertexScalars)
    {
      _vertexScalars = scalars;
      modified();
    }
  }

  Vectors* vertexVectors() const
  {
    return _vertexVectors;
  }

  void setVertexVectors(const Vectors* vectors)
  {
    if (vectors != _vertexVectors)
    {
      _vertexVectors = vectors;
      modified();
    }
  }

  int32_t cellCount() const
  {
    return 0;
  }

  Scalars* cellScalars() const
  {
    return _cellScalars;
  }

  void setCellScalars(const Scalars* scalars)
  {
    if (scalars != _cellScalars)
    {
      _cellScalars = scalars;
      modified();
    }
  }

  Vectors* cellVectors() const
  {
    return _cellVectors;
  }

  void setCellVectors(const Vectors* vectors)
  {
    if (vectors != _cellVectors)
    {
      _cellVectors = vectors;
      modified();
    }
  }

protected:
  Reference<Scalars> _vertexScalars;
  Reference<Scalars> _cellScalars;
  Reference<Vectors> _vertexVectors;
  Reference<Vectors> _cellVectors;

  void cloneAttributes(const DataSet& other)
  {
    _vertexScalars = other._vertexScalars;
    _cellScalars = other._cellScalars;
    _vertexVectors = other._vertexVectors;
    _cellVectors = other._cellVectors;
  }

}; // DataSet

} // end namespace cg::vis

#endif // __DataSet_h
