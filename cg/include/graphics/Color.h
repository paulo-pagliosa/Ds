//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2023 Paulo Pagliosa.                        |
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
// OVERVIEW: Color.h
// ========
// Class definition for RGB color.
//
// Author: Paulo Pagliosa
// Last revision: 30/07/2023

#ifndef __Color_h
#define __Color_h

#include "math/Vector4.h"
#include <cstdint>

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Color: RGB color class
// =====
class Color
{
public:
  union
  {
    struct
    {
      float r;
      float g;
      float b;
      float a;
    };

    struct
    {
      float x;
      float y;
      float z;
      float w;
    };
  };

  /// Default constructor.
  HOST DEVICE
  Color()
  {
    // do nothing
  }

  /// Constructs a Color object from (r, g, b, a).
  HOST DEVICE
  explicit Color(float r, float g, float b, float a = 1)
  {
    setRGB(r, g, b, a);
  }

  /// Constructs a Color object from c[4].
  HOST DEVICE
  explicit Color(const float* c)
  {
    setRGB(c);
  }

  /// Constructs a Color object from (r, g, b, a).
  HOST DEVICE
  explicit Color(int r, int g, int b, int a = 255)
  {
    setRGB(r, g, b, a);
  }

  /// Constructs a Color object from v.
  template <typename V>
  HOST DEVICE
  explicit Color(const V& v)
  {
    setRGB(v);
  }

  /// Sets this object to (r, g, b, a).
  HOST DEVICE
  void setRGB(float r, float g, float b, float a = 1)
  {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
  }

  /// Sets this object from c[4].
  HOST DEVICE
  void setRGB(const float* c)
  {
    r = c[0];
    g = c[1];
    b = c[2];
    a = c[3];
  }

  /// Sets this object from (r, g, b, a).
  HOST DEVICE
  void setRGB(int r, int g, int b, int a = 255)
  {
    this->r = r * math::inverse<float>(255);
    this->g = g * math::inverse<float>(255);
    this->b = b * math::inverse<float>(255);
    this->a = a * math::inverse<float>(255);
  }

  /// Sets this object from v.
  template <typename V>
  HOST DEVICE
  void setRGB(const V& v)
  {
    r = float(v.x);
    g = float(v.y);
    b = float(v.z);
    a = float(v.w);
  }

  template <typename V>
  HOST DEVICE
  auto& operator =(const V& v)
  {
    setRGB(v);
    return *this;
  }

  /// Returns this object + c.
  HOST DEVICE
  auto operator +(const Color& c) const
  {
    return Color{r + c.r, g + c.g, b + c.b};
  }

  /// Returns this object - c.
  HOST DEVICE
  auto operator -(const Color& c) const
  {
    return Color{r - c.r, g - c.g, b - c.b};
  }

  /// Returns this object * c.
  HOST DEVICE
  auto operator *(const Color& c) const
  {
    return Color{r * c.r, g * c.g, b * c.b};
  }

  /// Returns this object * s.
  HOST DEVICE
  auto operator *(float s) const
  {
    return Color{r * s, g * s, b * s};
  }

  /// Returns the i-th component of this object.
  HOST DEVICE
  const auto& operator [](int i) const
  {
    return (&r)[i];
  }

  /// Returns a reference to the i-th component of this object.
  HOST DEVICE
  auto& operator [](int i)
  {
    return (&r)[i];
  }

  /// Returns a pointer to the elements of this object.
  HOST DEVICE
  explicit operator const float* () const
  {
    return &r;
  }

  /// Returns a reference to this object += c.
  HOST DEVICE
  auto& operator +=(const Color& c)
  {
    r += c.r;
    g += c.g;
    b += c.b;
    return *this;
  }

  /// Returns a reference to this object -= c.
  HOST DEVICE
  auto& operator -=(const Color& c)
  {
    r -= c.r;
    g -= c.g;
    b -= c.b;
    return *this;
  }

  /// Returns a reference to this object *= c.
  HOST DEVICE
  auto& operator *=(const Color& c)
  {
    r *= c.r;
    g *= c.g;
    b *= c.b;
    return *this;
  }

  /// Returns a reference to this object *= s.
  HOST DEVICE
  auto& operator *=(float s)
  {
    r *= s;
    g *= s;
    b *= s;
    return *this;
  }

  /// Returns true if this object is equals to c.
  HOST DEVICE
  bool equals(const Color& c, float eps = math::Limits<float>::eps()) const
  {
    return math::isNull(r - c.r, g - c.g, b - c.b, eps);
  }

  HOST DEVICE
  bool operator ==(const Color& c) const
  {
    return equals(c);
  }

  /// Returns true if this object is not equals to c.
  HOST DEVICE
  bool operator !=(const Color& c) const
  {
    return !operator ==(c);
  }

  void print(const char* s) const
  {
    printf("%srgb(%g,%g,%g)\n", s, r, g, b);
  }

  static Color black;
  static Color red;
  static Color green;
  static Color blue;
  static Color cyan;
  static Color magenta;
  static Color yellow;
  static Color white;
  static Color darkGray;
  static Color gray;

  static Color HSV2RGB(float, float, float, float = 1);

}; // Color

/// Returns the color s * c.
template <typename real>
HOST DEVICE inline auto
operator *(real s, const Color& c)
{
  return c * float(s);
}

#define R_SHIFT 0x00u
#define G_SHIFT 0x08u
#define B_SHIFT 0x10u
#define A_SHIFT 0x18u

constexpr inline uint32_t
packColor(uint32_t r, uint32_t g, uint32_t b, uint32_t a = 255)
{
  return a << A_SHIFT | b << B_SHIFT | g << G_SHIFT | r << R_SHIFT;
}

inline uint32_t
packColor(const Color& c)
{
  const auto r = uint32_t(c.r * 255);
  const auto g = uint32_t(c.g * 255);
  const auto b = uint32_t(c.b * 255);
  const auto a = uint32_t(c.a * 255);

  return packColor(r, g, b, a);
}

inline Color
unpackColor(uint32_t c)
{
  auto r = ((c >> R_SHIFT) & 0xFF) * math::inverse<float>(255);
  auto g = ((c >> G_SHIFT) & 0xFF) * math::inverse<float>(255);
  auto b = ((c >> B_SHIFT) & 0xFF) * math::inverse<float>(255);
  auto a = ((c >> A_SHIFT) & 0xFF) * math::inverse<float>(255);

  return Color{r, g, b, a};
}

#undef R_SHIFT
#undef G_SHIFT
#undef B_SHIFT
#undef A_SHIFT

} // end namespace cg

#endif // __Color_h
