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
// OVERVIEW: Glyph3.h
// ========
// Class definition for vis 3D glyph filter.
//
// Author: Paulo Pagliosa
// Last revision: 15/03/2022

#ifndef __Glyph3_h
#define __Glyph3_h

#include "Filter.h"
#include "PolyMeshBuilder.h"
#include "PointSet.h"

namespace cg::vis
{ // begin namespace cg::vis

enum class GlyphScaleMode
{
  Scalar,
  Vector
};


/////////////////////////////////////////////////////////////////////
//
// Glyph3Base: vis 3D glyph base class
// ==========
class Glyph3Base: public virtual Object, public PolyMeshBuilder
{
public:
  constexpr static auto minScaleFactor = 0.001f;

  PolyMeshGeometry* source() const
  {
    return _source;
  }

  void setSource(const PolyMeshGeometry* source)
  {
    if (_source != source)
    {
      _source = source;
      modified();
    }
  }

  void range(float& min, float& max) const
  {
    min = _range[0];
    max = _range[1];
  }

  void setRange(float min, float max);

  auto scaleFactor() const
  {
    return _scaleFactor;
  }

  void setScaleFactor(float value)
  {
    _scaleFactor = math::max(minScaleFactor, value);
    modified();
  }

  auto scaleMode() const
  {
    return _scaleMode;
  }

  void setScaleMode(GlyphScaleMode scaleMode)
  {
    if (scaleMode != _scaleMode)
    {
      _scaleMode = scaleMode;
      modified();
    }
  }

  auto clamping() const
  {
    return _clamping;
  }

  void setClamping(bool clamping)
  {
    if (clamping != _clamping)
    {
      _clamping = clamping;
      modified();
    }
  }

protected:
  Glyph3Base();

  Reference<PolyMeshGeometry> makeDefaultSource() const;
  void execute(const PointSet&);

private:
  Reference<PolyMeshGeometry> _source;
  float _range[2];
  float _scaleFactor;
  GlyphScaleMode _scaleMode;
  bool _clamping;

}; // Glyph3Base


/////////////////////////////////////////////////////////////////////
//
// Glyph3: vis 3D glyph filter class
// ======
template <typename Input>
class Glyph3: public Filter<Input, PolyMesh>, public Glyph3Base
{
public:
  static Reference<Glyph3<Input>> New()
  {
    return new Glyph3<Input>;
  }

  void execute() override;

private:
  Glyph3() = default;

}; // Glyph3

template <typename Input>
void
Glyph3<Input>::execute()
{
  auto input = this->input();

  if (auto nv = input->vertexCount(); nv > 0)
  {
    auto points = PointSet::New(nv);

    for (decltype(nv) i = 0; i < nv; ++i)
      points->set(i, input->vertex(i));
    points->setVertexScalars(input->vertexScalars());
    points->setVertexVectors(input->vertexVectors());
    Glyph3Base::execute(*points);
  }
  this->setOutput(this->mesh());
}

} // end namespace cg::vis

#endif // __Glyph3_h
