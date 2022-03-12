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
// OVERVIEW: Mapper.h
// ========
// Class definition for generic vis mapper.
//
// Author: Paulo Pagliosa
// Last revision: 11/03/2022

#ifndef __Mapper_h
#define __Mapper_h

#include "graphics/GLRenderer.h"
#include "graphics/PrimitiveMapper.h"
#include "LookupTable.h"
#include "Filter.h"
#include "Scalars.h"

namespace cg::vis
{ // begin namespace cg>::vis


/////////////////////////////////////////////////////////////////////
//
// AbstractMapper: vis abstract mapper class
// ==============
class AbstractMapper: public PrimitiveMapper, public virtual Object
{
public:
  bool useVertexColors{true};
  bool drawBounds{false};

  virtual const char* name() const = 0;

  LookupTable* lookupTable() const
  {
    return _lookupTable;
  }

  void setLookupTable(LookupTable* lookupTable);

  Bounds3f bounds() const final;
  Primitive* primitive() const final;

protected:
  mutable Bounds3f _bounds;
  mutable Timestamp _buildTime;

  const auto& colorMap() const
  {
    return _colorMap;
  }

  virtual bool mapColors(Scalars*) const;
  virtual bool draw(GLRenderer&) const = 0;

private:
  mutable std::vector<Color> _colorMap;
  mutable Reference<LookupTable> _lookupTable;

  void updateLookupTableRange(Scalars&) const;

}; // AbstractMapper

inline auto
asAbstractMapper(PrimitiveMapper* mapper)
{
  return dynamic_cast<AbstractMapper*>(mapper);
}


/////////////////////////////////////////////////////////////////////
//
// Mapper: generic vis mapper class
// ======
template <typename Input>
class Mapper: public AbstractMapper
{
public:
  Input* input() const
  {
    return _inputConnection.data();
  }

  void setInputConnection(OutputPort<Input>* port)
  {
    _inputConnection.setOutputPort(port);
  }

  void setInput(Source<Input>& input)
  {
    setInputConnection(input.outputPort());
  }

  void update() override;
  bool render(GLRenderer&) const override;

private:
  InputConnection<Input> _inputConnection;

}; // Mapper

template <typename Input>
void
Mapper<Input>::update()
{
  if (_inputConnection.isSet())
    _inputConnection.outputPort()->source()->update();
}

template <typename Input>
bool
Mapper<Input>::render(GLRenderer& renderer) const
{
  const auto input = this->input();

  if (input == nullptr)
    return false;
  if (this->_buildTime < input->modifiedTime())
  {
    this->_bounds = input->bounds();
    this->mapColors(input->vertexScalars());
    this->_buildTime.modified();
  }
  renderer.flags.enable(GLRenderer::DrawBounds, this->drawBounds);
  return this->draw(renderer);
}

} // end namespace cg::vis

#endif // __Mapper_h
