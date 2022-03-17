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
// Last revision: 16/03/2022

#include "geometry/MeshSweeper.h"
#include "Glyph3.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// Glyph3Base implementation
// ==========
Glyph3Base::Glyph3Base():
  _scaleMode{GlyphScaleMode::Scalar},
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

  // Arrow
  t->setScale({0.01f, 0.4f, 0.01f});
  g->addElement(*MeshSweeper::makeCylinder());
  t->set({0, 0.4f, 0}, quatf::identity(), {0.05f, 0.2f, 0.05f});
  g->addElement(*MeshSweeper::makeCone());
  return g;
}

void
Glyph3Base::execute(const PointSet& points)
{
  auto np = points.size();

  if (np == 0)
  {
#ifdef _DEBUG
    puts("Glyph3: no points");
#endif // _DEBUG
    return;
  }

  auto vectors = points.vertexVectors();

  if (vectors == nullptr)
  {
#ifdef _DEBUG
    puts("Glyph3: no vector field");
#endif // _DEBUG
    return;
  }
  else if (np != vectors->size())
    throw std::logic_error("Glyph3: bad vector field");

  auto scalars = points.vertexScalars();

  if (scalars == nullptr)
  {
    if (_scaleMode == GlyphScaleMode::Scalar)
    {
#ifdef _DEBUG
      puts("Glyph3: no scalar field");
#endif // _DEBUG
      return;
    }
  }
  else if (np != scalars->size())
    throw std::logic_error("Glyph3: bad scalar field");
  if (_source == nullptr)
    _source = makeDefaultSource();
  for(decltype(np) i = 0; i < np; ++i)
  {
    const auto& v = vectors->get(i);
    auto scale = v.length();

    if (math::isZero(scale))
      continue;

    auto rotation = quatf::identity();

    if (!math::isZero(v.x) || !math::isZero(v.z))
      rotation.set(180, {v.x, v.y + scale, v.z});
    else if (v.y < 0)
      rotation.set(180, {0, 0, 1});
    if (_scaleMode == GlyphScaleMode::Scalar)
    {
      scale = scalars->get(i);
      if (_clamping)
      {
        auto d = _range[1] - _range[0];

        if (math::isZero(d))
          d = 1.0f;
        scale = math::clamp(scale, _range[0], _range[1]);
        scale = (scale - _range[0]) / d;
      }
    }
    scale *= _scaleFactor;
    if (scale <= 0)
      scale = 1e-6f;
    transform()->set(points.get(i), rotation, vec3f{scale});
    makeInstance(*_source);
  }
  mesh()->setVertexScalars(scalars);
  mesh()->setVertexVectors(vectors);
}

} // end namespace cg::vis
