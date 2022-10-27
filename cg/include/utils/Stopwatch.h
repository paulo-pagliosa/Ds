//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019, 2022 Paulo Pagliosa.                        |
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
// OVERVIEW: Stopwatch.h
// ========
// Class definition for stopwatch.
//
// Author: Paulo Pagliosa
// Last revision: 26/10/2022

#ifndef __Stopwatch_h
#define __Stopwatch_h

#include <chrono>

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Stopwatch: stopwatch class
// =========
class Stopwatch
{
public:
  using ms = std::chrono::duration<double, std::milli>;
  using ms_time = decltype(ms{}.count());

  void start()
  {
    if (!_running)
    {
      auto ct = _ct;

      _ct = now();
      _st += _ct - ct;
      _lt += _ct - ct;
      _running = true;
    }
  }

  void stop()
  {
    _running = false;
  }

  auto time()
  {
    if (_running)
      _ct = now();
    return time(_st, _ct);
  }

  auto lap()
  {
    if (!_running)
      return ms_time{};
    _ct = now();

    auto et = time(_lt, _ct);

    _lt = _ct;
    return et;
  }

  void reset()
  {
    _lt = _st = _ct;
    _running = false;
  }

private:
  using clock_type = std::chrono::steady_clock;
  using time_point = decltype(clock_type::now());

  time_point _st{};
  time_point _ct{};
  time_point _lt{};
  bool _running{false};

  static time_point now()
  {
    return clock_type::now();
  }

  static ms_time time(const time_point& t1, const time_point& t2)
  {
    return std::chrono::duration_cast<ms>(t2 - t1).count();
  }

}; // Stopwatch

} // end namespace cg

#endif // __Stopwatch_h
