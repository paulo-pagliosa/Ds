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
// OVERVIEW: Writer.h
// ========
// Class definition for generic vis writer.
//
// Author: Paulo Pagliosa
// Last revision: 11/03/2022

#ifndef __Writer_h
#define __Writer_h

#include "Filter.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// Writer: generic vis writer class
// ======
template <typename Input>
class Writer: public virtual Object
{
public:
  void setInputConnection(OutputPort<Input>* port)
  {
    _inputConnection.setOutputPort(port);
  }

  void setInputConnection(Source<Input>& input)
  {
    setInputConnection(input.outputPort());
  }

  void update()
  {
    write();
  }

  virtual void write();

protected:
  Input* input() const
  {
    return _inputConnection.data();
  }

  virtual void start();
  virtual void writeData() = 0;
  virtual void end();

private:
  InputConnection<Input> _inputConnection;

}; // Writer

template <typename Input>
void
Writer<Input>::start()
{
  // do nothing
}

template <typename Input>
void
Writer<Input>::write()
{
  if (!_inputConnection.isSet())
    std::runtime_error("Writer::write(): no input");

  auto input = _inputConnection.outputPort()->source();

  input->update();
  this->start();
  this->writeData();
  this->end();
}

template <typename Input>
void
Writer<Input>::end()
{
  // do nothing
}

} // end namespace cg::vis

#endif // __Writer_h
