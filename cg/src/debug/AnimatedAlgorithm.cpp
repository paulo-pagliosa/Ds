//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2016, 2019 Paulo Pagliosa.                        |
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
// OVERVIEW: AnimatedAlgorithm.cpp
// ========
// Source file for animated algorithm.
//
// Author: Paulo Pagliosa
// Last revision: 12/02/2019

#include "debug/AnimatedAlgorithm.h"
#include <cstdarg>
#include <stdexcept>

namespace cg
{ // begin namespace cg

#ifdef _DRAW_ALG

// TODO: fix the draw function

namespace this_algorithm
{ // begin namespace this_algorithm

class AbortException: public std::runtime_error
{
public:
  AbortException():
    std::runtime_error("aborted")
  {
    // do nothing
  }

}; // AbortException

void
draw(bool stop, const char* fmt, ...)
{
  if (auto a = AnimatedAlgorithm::_current)
  {
    if (a->_state == AnimatedAlgorithm::State::CANCEL)
      throw AbortException{};
    if (!GUI::isPoolingEvents())
      return;
    if (fmt != nullptr)
    {
      const size_t maxLen{1024};
      char msg[maxLen];
      va_list args;

      va_start(args, fmt);
      vsnprintf(msg, maxLen, fmt, args);
      a->_stepLog = msg;
    }
    a->wait(a->runMode == AnimatedAlgorithm::RunMode::STEP || stop);
  }
}

} // end namespace this_algorithm

#endif


/////////////////////////////////////////////////////////////////////
//
// AnimatedAlgoritm implementation
// ================
AnimatedAlgorithm* AnimatedAlgorithm::_current;

inline void
AnimatedAlgorithm::wait(bool stop)
{
  std::unique_lock<std::mutex> lock{_lockDrawing};

  _stopped = stop;
  _canDraw = !(_notified = false);
  _state = State::SLEEPING;
  _drawing.wait(lock, [this]() { return _notified; });
}

void
AnimatedAlgorithm::wake(bool force)
{
  if ((!_notified && !_stopped) || force)
  {
    _canDraw = !(_notified = true);
    if (_state != State::CANCEL)
      _state = State::RUNNING;
    _drawing.notify_one();
  }
}

void
AnimatedAlgorithm::launch()
{
  _thread = ThreadPtr{new std::thread{[this]()
  {
    _state = State::RUNNING;
    wait(true);
    try
    {
      if (_state != State::CANCEL)
        run();
    }
    catch (...)
    {
      // do nothing
    }
    _canDraw = true;
    _stepLog.clear();
    _state = State::RUN;
  }}};
}

void
AnimatedAlgorithm::start()
{
  if (_state != State::CREATED && _state != State::TERMINATED)
    return;
  _current = this;
  // Initialize this algorithm.
  initialize();
  // Run this algorithm in a new thread.
  launch();
}

void
AnimatedAlgorithm::join()
{
  if (_state != State::RUN)
    _state = State::CANCEL;
  // Wait for thread termination.
  wake(true);
  _thread->join();
  _thread = nullptr;
  // Terminate this algorithm.
  _state = State::TERMINATED;
  terminate();
  _current = nullptr;
}

void
AnimatedAlgorithm::cancel()
{
  if (_state == State::RUNNING)
    _state = State::CANCEL;
  else if (_state == State::SLEEPING)
  {
    _state = State::CANCEL;
    wake(true);
  }
}

} // end namespace cg
