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
// OVERVIEW: Filter.h
// ========
// Class definition for generic vis filter.
//
// Author: Paulo Pagliosa
// Last revision: 08/03/2022

#ifndef __Filter_h
#define __Filter_h

#include "Source.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// InputConnection: input connection class
// ===============
template <typename T>
class InputConnection
{
public:
  auto isSet() const
  {
    return _outputPort != nullptr;
  }

  ~InputConnection()
  {
    if (isSet())
      Object::release(_outputPort->source());
  }

  auto data() const
  {
    return isSet() ? _outputPort->data() : nullptr;
  }

  auto outputPort() const
  {
    return _outputPort;
  }

  void setOutputPort(OutputPort<T>* port);

private:
  OutputPort<T>* _outputPort{};

}; // InputConnection

template <typename T>
void
InputConnection<T>::setOutputPort(OutputPort<T>* port)
{
  if (port == _outputPort)
    return;
  if (_outputPort != nullptr)
    Object::release(_outputPort->source());
  _outputPort = port;
  if (port != nullptr)
    Object::makeUse(port->source());
}


/////////////////////////////////////////////////////////////////////
//
// Filter: generic vis filter class
// ======
template <typename Input, typename Output>
class Filter: public Source<Output>
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

private:
  InputConnection<Input> _inputConnection;

}; // Filter

template <typename Input, typename Output>
void
Filter<Input, Output>::update()
{
  if (!_inputConnection.isSet())
    std::runtime_error("Filter::update(): no input");

  auto input = _inputConnection.outputPort()->source();

  input->update();
  if (this->_executeTime < input->modifiedTime() ||
    this->_executeTime < this->modifiedTime())
  {
    this->start();
    this->execute();
    this->end();
    this->_executeTime.modified();
  }
}

} // end namespace cg::vis

#endif // __Filter_h
