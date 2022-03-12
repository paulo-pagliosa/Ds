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
// OVERVIEW: Source.h
// ========
// Class definition for generic vis source.
//
// Author: Paulo Pagliosa
// Last revision: 11/03/2022

#ifndef __Source_h
#define __Source_h

#include "core/Exception.h"
#include "Object.h"
#include <stdexcept>

namespace cg::vis
{ // begin namespace cg::vis

//
// Forward definition
//
template <typename Output> class Source;


/////////////////////////////////////////////////////////////////////
//
// AbstractSource: abstract vis source class
// ==============
class AbstractSource: public virtual Object
{
public:
  virtual void update();

protected:
  Timestamp _executeTime;

  virtual void start();
  virtual void execute() = 0;
  virtual void end();

}; // AbstractSource


/////////////////////////////////////////////////////////////////////
//
// OutputPort: output port class
// ==========
template <typename T>
class OutputPort
{
public:
  OutputPort(Source<T>& source):
    _source{&source}
  {
    // do nothing
  }

  Source<T>* source() const
  {
    return _source;
  }

  T* data() const
  {
    return _data;
  }

  void setData(const T* data)
  {
    _data = data;
  }

private:
  Source<T>* _source;
  Reference<T> _data;

}; // OutputPort


/////////////////////////////////////////////////////////////////////
//
// Source: generic vis source class
// ======
template <typename Output>
class Source: public AbstractSource
{
public:
  auto output() const
  {
    return _outputPort.data();
  }

  const OutputPort<Output>* outputPort() const
  {
    return &_outputPort;
  }

  OutputPort<Output>* outputPort()
  {
    return &_outputPort;
  }

protected:
  Source():
    _outputPort{*this}
  {
    // do nothing
  }

  void setOutput(const Output* data)
  {
    _outputPort.setData(data);
  }

  void execute() override;

private:
  OutputPort<Output> _outputPort;

}; // Source

template <typename Output>
void
Source<Output>::execute()
{
  throw bad_invocation("Source<T>", __func__);
}

} // end namespace cg::vis

#endif // __Source_h
