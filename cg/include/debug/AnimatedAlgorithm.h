//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2016, 2022 Paulo Pagliosa.                        |
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
// OVERVIEW: AnimatedAlgorithm.h
// ========
// Class definition for animated algorithm.
//
// Author: Paulo Pagliosa
// Last revision: 10/02/2022

#ifndef __AnimatedAlgorithm_h
#define __AnimatedAlgorithm_h

#include <condition_variable>
#include <memory>
#include <string>
#include <thread>

namespace cg
{ // begin namespace cg

namespace this_algorithm
{ // begin namespace this_algorithm

#ifdef _DRAW_ALG

 /// Draws the current algorithm.
void draw(bool stop = false, const char* fmt = nullptr, ...);

#else

inline void
draw(bool, const char*, ...)
{
  // do nothing
}

#endif // _DRAW_ALG

} // end namespace this_algorithm


/////////////////////////////////////////////////////////////////////
//
// AnimatedAlgorithm: animated algorithm class
// =================
class AnimatedAlgorithm
{
public:
  enum class State
  {
    CREATED,
    RUNNING,
    SLEEPING,
    CANCEL,
    RUN,
    TERMINATED
  };

  enum class RunMode
  {
    CONTINUE,
    STEP
  };

  RunMode runMode{RunMode::CONTINUE};

  /// Destructor.
  virtual ~AnimatedAlgorithm()
  {
    // do nothing
  }

  /// Gets the name of this algorithm.
  const char* name() const
  {
    return _name.c_str();
  }

  State state() const
  {
    return _state;
  }

  /// Starts this algorithm.
  void start();

  /// Waits for termination of this algorithm.
  void join();

  const std::string& stepLog() const
  {
    return _stepLog;
  }

protected:
  bool _canDraw{false};
  bool _stopped{false};

  /// Constructor.
  AnimatedAlgorithm(const std::string& s):
    _name{s},
    _state{State::CREATED}
  {
    // do nothing
  }

  /// Initializes this algorithm.
  virtual void initialize()
  {
    // do nothing
  }

  /// Runs this algorithm.
  virtual void run() = 0;

  /// Terminates this algorithm.
  virtual void terminate()
  {
    // do nothing
  }

  /// Handles mouse button event.
  virtual bool onMouse(double, double, int, int)
  {
    return false;
  }
  
  /// Handles mouse motion event.
  virtual bool onMotion(int, double, double)
  {
    return false;
  }

  /// Handles mouse wheel/touchpad gesture event.
  virtual bool onScroll(double, double)
  {
    return false;
  }

  /// Handles text input event.
  virtual bool onChar(unsigned int)
  {
    return false;
  }

  virtual void draw(int, int) = 0;

  void wait(bool);
  void wake(bool = false);

  void stop(bool stop = true)
  {
    if (!(_stopped = stop))
      wake();
  }

  void cancel();

private:
  using ThreadPtr = std::unique_ptr<std::thread>;

  std::string _name;
  State _state;
  std::mutex _lockDrawing;
  std::condition_variable _drawing;
  ThreadPtr _thread;
  bool _notified{true};
  std::string _stepLog;

  static AnimatedAlgorithm* _current;

  void launch();

  friend class GUI;
  friend void this_algorithm::draw(bool, const char*, ...);

}; // AnimatedAlgorithm

} // end namespace cg

#endif // __AnimatedAlgorithm_h
