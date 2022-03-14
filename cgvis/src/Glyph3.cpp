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
// OVERVIEW: Glyph3.cpp
// ========
// Source file for vis 3D glyph filter.
//
// Author: Paulo Pagliosa
// Last revision: 14/03/2022

#include "geometry/MeshSweeper.h"
#include "Glyph3.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// Glyph3Base implementation
// ==========
Glyph3Base::Glyph3Base():
  _scaleMode{ScaleMode::Scalar},
  _scaleFactor{1},
  _clamping{false}
{
  _range[0] = 0;
  _range[1] = 1;
}

void
Glyph3Base::setRange(float min, float max)
{
  if (min > max)
    math::swap(min, max);
  _range[0] = min;
  _range[1] = max;
  modified();
}

Reference<PolyMeshGeometry>
Glyph3Base::makeDefaultSource() const
{
  auto g = PolyMeshGeometry::New();
  auto t = g->transform();

  t->setScale({1, 0.001f, 1});
  g->addElement(*MeshSweeper::makeCylinder());
  t->set({0, 1, 0}, quatf::identity(), vec3f{0.3f});
  g->addElement(*MeshSweeper::makeCone());
  return g;
}

void
Glyph3Base::execute(const Points& points, PolyMesh& output)
{
  if (_source == nullptr)
    _source = makeDefaultSource();
  /*
  tGraphicModel* source = 0;
  tPolygonExtractor pe;

  if (Source)
  {
    pe.SetInput(Source);
    pe.Execute();
    source = pe.GetOutput();
  }
  if (source == 0)
    return;

  t3DTransfMatrix t;
  t3DTransfMatrix temp;

  for (auto& p : points)
  {
    tGraphicModel* glyph = source->MakeObject(*Output);
    t3DPoint* point = pit.Current();

    // translate source to Input point
    t.Translate(point->Position);

    t3DVector v = point->Vector;
    double scale = v.Length();
    bool scaleSourceFlag = false;

    if (scale > 0)
    {
      // if there is no x or y component
      if (IsZero(v.x) && IsZero(v.y))
      {
        // just flip z if we need to
        if (v.z < 0)
        {
          temp.Rotate(t3DVector::YAxis, ToRadians(180));
          t *= temp;
        }
      }
      else
      {
        t3DVector axis(v.x * 0.5, v.y * 0.5, (v.z + scale) * 0.5);

        temp.Rotate(axis, ToRadians(180));
        t *= temp;
      }
      scaleSourceFlag = true;
    }
    // determine scale factor from scalars if appropriate
    if (ScaleMode == ScaleByScalar)
    {
      // scale = point->Scalar;
      if (Clamping)
      {
        double d = Range[1] - Range[0];

        if (IsZero(d))
          d = 1.0;
        scale = scale < Range[0] ? Range[0] :
          (scale > Range[1] ? Range[1] : scale);
        scale = (scale - Range[0]) / d;
      }
      scaleSourceFlag = true;
    }
    // scale data if appropriate
    if (scaleSourceFlag)
    {
      scale *= ScaleFactor;
      if (scale == 0.0)
        scale = 1.0e-10;
      temp.Scale(t3DVector(scale, scale, scale));
      t *= temp;
    }
    glyph->Transform(t);
  }
  */
}

} // end namespace cg::vis
