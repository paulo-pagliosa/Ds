//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2019 Orthrus Group.                         |
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
// Last revision: 27/05/2019

#ifndef __Flags_h
#define __Flags_h

#include "core/Globals.h"
#include <cstdint>

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Flags: flags class
// =====
template <typename Bits>
class Flags
{
public:
  /// Contructs a Flags object with no bits set.
  HOST DEVICE
  Flags()
  {
    bits = 0;
  }
  
  /// Constructs a Flags object initialized with mask.
  HOST DEVICE
  Flags(Bits mask)
  {
    bits = (uint32_t)mask;
  }

  /// \brief Assigns mask to the bits of this object.
  HOST DEVICE
  Flags<Bits>& operator =(Bits mask)
  {
    bits = (uint32_t)mask;
    return *this;
  }

  HOST DEVICE
  Flags<Bits> operator |(Bits mask) const
  {
    return Flags<Bits>{bits | (uint32_t)mask};
  }

  HOST DEVICE
  Flags<Bits> operator |(Flags<Bits> flags) const
  {
    return Flags<Bits>{bits | flags.bits};
  }

  HOST DEVICE
  Flags<Bits>& operator |=(Bits mask)
  {
    bits |= (uint32_t)mask;
    return *this;
  }

  HOST DEVICE
  Flags<Bits>& operator |=(Flags<Bits> flags)
  {
    bits |= flags.bits;
    return *this;
  }

  /// Sets the bits of this object, given by mask, to 1.
  HOST DEVICE
  void set(Bits mask)
  {
    bits |= (uint32_t)mask;
  }

  /// Sets the bits of this object, given by mask, to 0.
  HOST DEVICE
  void reset(Bits mask)
  {
    bits &= ~(uint32_t)mask;
  }

  /// Sets all bits of this object to 0.
  HOST DEVICE
  void clear()
  {
    bits = 0;
  }

  /// Sets the bits of this object, given by mask, to state.
  HOST DEVICE
  void enable(Bits mask, bool state)
  {
    state ? set(mask) : reset(mask);
  }

  /// Converts this object to int.
  HOST DEVICE
  operator uint32_t() const
  {
    return bits;
  }

  /// Returns true if all bits of this object, given by mask, are set.
  HOST DEVICE
  bool isSet(Bits mask) const
  {
    return (bits & (uint32_t)mask) == (uint32_t)mask;
  }

  /// Returns true if any bits of this object, given by mask, are set.
  HOST DEVICE
  bool test(Bits mask) const
  {
    return (bits & (uint32_t)mask) != 0;
  }

private:
  uint32_t bits{};

}; // Flags

} // end namespace cg

#endif // __Flags_h
