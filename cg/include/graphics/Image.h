//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2026 Paulo Pagliosa.                        |
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
// OVERVIEW: Image.h
// ========
// Class definition for generic image.
//
// Author: Paulo Pagliosa
// Last revision: 04/09/2026

#ifndef __Image_h
#define __Image_h

#include "core/SharedObject.h"
#include "graphics/Color.h"

namespace cg
{ // begin namespace cg

constexpr auto minImageWidth = 4;

[[nodiscard]] constexpr auto
roundupImageWidth(int w)
{
  return (w + minImageWidth - 1) & -minImageWidth;
}

//
// Forward definition
//
class Image;


/////////////////////////////////////////////////////////////////////
//
// Pixel: pixel class
// =====
struct Pixel
{
  using byte = uint8_t;

  byte r;
  byte g;
  byte b;

  constexpr Pixel() = default;

  HOST DEVICE
  constexpr Pixel(byte r, byte g, byte b)
  {
    set(r, g, b);
  }

  HOST DEVICE
  constexpr Pixel(const Color& c)
  {
    set(c);
  }

  HOST DEVICE
  constexpr void set(byte r, byte g, byte b)
  {
    this->r = r;
    this->g = g;
    this->b = b;
  }

  HOST DEVICE
  constexpr void set(const Color& c)
  {
    r = (byte)(255 * c.r);
    g = (byte)(255 * c.g);
    b = (byte)(255 * c.b);
  }
  
  [[nodiscard]] HOST DEVICE
  constexpr auto operator +(const Pixel& p) const
  {
    return Pixel{byte(r + p.r), byte(g + p.g), byte(b + p.b)};
  }

  [[nodiscard]] HOST DEVICE
  constexpr auto operator +(const Color& c) const
  {
    return operator +(Pixel{c});
  }

  HOST DEVICE
  constexpr auto& operator +=(const Pixel& p)
  {
    r += p.r;
    g += p.g;
    b += p.b;
    return *this;
  }

  HOST DEVICE
  constexpr auto& operator +=(const Color& c)
  {
    return operator +=(Pixel{c});
  }

}; // Pixel


/////////////////////////////////////////////////////////////////////
//
// ImageBuffer: generic image buffer class
// ===========
class ImageBuffer
{
public:
  // Default constructor.
  ImageBuffer() = default;

  // Constructor.
  ImageBuffer(int width, int height);

  ImageBuffer(const ImageBuffer&) = delete;
  ImageBuffer& operator =(const ImageBuffer&) = delete;

  // Move constructor and operator.
  ImageBuffer(ImageBuffer&& other) noexcept;
  ImageBuffer& operator =(ImageBuffer&& other) noexcept;

  // Destructor.
  ~ImageBuffer()
  {
    delete []_data;
  }

  [[nodiscard]] auto width() const
  {
    return _W;
  }

  [[nodiscard]] auto height() const
  {
    return _H;
  }

  [[nodiscard]] const auto data() const
  {
    return _data;
  }

  [[nodiscard]] const auto& operator ()(int x, int y) const
  {
    assert(x >= 0 && x < _W && y >= 0 && y < _H);
    return _data[y * _W + x];
  }

  [[nodiscard]] auto& operator ()(int x, int y)
  {
    assert(x >= 0 && x < _W && y >= 0 && y < _H);
    return _data[y * _W + x];
  }

  [[nodiscard]] auto length() const
  {
    return _W * _H;
  }

  [[nodiscard]] const auto& operator [](int i) const
  {
    assert(i >= 0 && i < _W * _H);
    return _data[i];
  }

  [[nodiscard]] auto& operator [](int i)
  {
    assert(i >= 0 && i < _W * _H);
    return _data[i];
  }

private:
  int _W{};
  int _H{};
  Pixel* _data{};

  friend Image;

}; // ImageBuffer


/////////////////////////////////////////////////////////////////////
//
// Image: generic image class
// =====
class Image: public SharedObject
{
public:
  [[nodiscard]] auto width() const
  {
    return _W;
  }

  [[nodiscard]] auto height() const
  {
    return _H;
  }

  void setData(int x, int y, const ImageBuffer& buffer);

  void setData(const ImageBuffer& buffer)
  {
    setData(0, 0, buffer);
  }

  [[nodiscard]] ImageBuffer data(int x, int y, int w, int h) const;

  [[nodiscard]] auto data() const
  {
    return data(0, 0, _W, _H);
  }

  virtual void draw(int x = 0, int y = 0) const = 0;

protected:
  int _W{};
  int _H{};

  // Default protected constructor.
  Image() = default;

  // Protected constructor.
  Image(int width, int height);

  virtual void setSubImage(int, int, int, int, const Pixel*) = 0;
  virtual void getSubImage(int, int, int, int, Pixel*) const = 0;

}; // Image

} // end namespace cg

#endif // __Image_h
