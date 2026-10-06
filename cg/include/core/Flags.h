//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2026 Paulo Pagliosa.                        |
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
// OVERVIEW: Flags.h
// ========
// Class definition for flags.
//
// Author: Paulo Pagliosa
// Last revision: 06/10/2026

#ifndef __Flags_h
#define __Flags_h

#include "core/Globals.h"
#include <cstdint>

namespace cg
{ // begin namespace cg

template <typename Bits>
[[nodiscard]] HOST DEVICE
constexpr auto
operator |(Bits a, Bits b)
{
  return Bits((uint32_t)a | (uint32_t)b);
}


/////////////////////////////////////////////////////////////////////
//
// Flags: flags class
// =====
template <typename Bits>
class Flags
{
public:
  /// Constructs a Flags object initialized with mask.
  HOST DEVICE
  constexpr Flags(Bits mask):
    _bits{(uint32_t)mask}
  {
    // do nothing
  }

  /// Contructs a Flags object with default mask.
  HOST DEVICE
  constexpr Flags():
    Flags{Bits{}}
  {
    // do nothing
  }

  /// \brief Assigns mask to the bits of this object.
  HOST DEVICE
  constexpr auto& operator =(Bits mask)
  {
    _bits = (uint32_t)mask;
    return *this;
  }

  [[nodiscard]] HOST DEVICE
  constexpr Flags operator |(Bits mask) const
  {
    return {_bits | (uint32_t)mask};
  }

  [[nodiscard]] HOST DEVICE
  constexpr Flags operator |(Flags flags) const
  {
    return {_bits | flags._bits};
  }

  HOST DEVICE
  constexpr auto& operator |=(Bits mask)
  {
    _bits |= (uint32_t)mask;
    return *this;
  }

  HOST DEVICE
  constexpr auto& operator |=(Flags flags)
  {
    _bits |= flags._bits;
    return *this;
  }

  /// Sets the bits of this object, given by mask, to 1.
  HOST DEVICE
  constexpr void set(Bits mask)
  {
    _bits |= (uint32_t)mask;
  }

  /// Sets the bits of this object, given by mask, to 0.
  HOST DEVICE
  constexpr void reset(Bits mask)
  {
    _bits &= ~(uint32_t)mask;
  }

  /// Sets all bits of this object to 0.
  HOST DEVICE
  constexpr void clear()
  {
    _bits = 0;
  }

  /// Sets the bits of this object, given by mask, to state.
  HOST DEVICE
  constexpr void enable(Bits mask, bool state)
  {
    state ? set(mask) : reset(mask);
  }

  /// Converts this object to int.
  [[nodiscard]] HOST DEVICE
  constexpr explicit operator uint32_t() const
  {
    return _bits;
  }

  /// Converts this object to Bits.
  [[nodiscard]] HOST DEVICE
  constexpr operator Bits() const
  {
    return static_cast<Bits>(_bits);
  }

  /// Returns true if all bits of this object, given by mask, are set.
  [[nodiscard]] HOST DEVICE
  constexpr bool isSet(Bits mask) const
  {
    return (_bits & (uint32_t)mask) == (uint32_t)mask;
  }

  /// Returns true if any bits of this object, given by mask, are set.
  [[nodiscard]] HOST DEVICE
  constexpr bool test(Bits mask) const
  {
    return (_bits & (uint32_t)mask) != 0;
  }

private:
  uint32_t _bits;

}; // Flags

} // end namespace cg

#endif // __Flags_h
