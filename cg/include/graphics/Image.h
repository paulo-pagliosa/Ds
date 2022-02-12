//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2020 Paulo Pagliosa.                        |
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
// Last revision: 29/06/2020

#ifndef __Image_h
#define __Image_h

#include "core/SharedObject.h"
#include "graphics/Color.h"
#include <stdexcept>

namespace cg
{ // begin namespace cg

#define MIN_IMAGE_WIDTH 4

inline int
roundupImageWidth(int w)
{
  return (w + MIN_IMAGE_WIDTH - 1) & -MIN_IMAGE_WIDTH;
}

static inline
void image_index_out_of_range()
{
  throw std::logic_error("Image: index out of range");
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

  HOST DEVICE
  Pixel()
  {
    // do nothing
  }

  HOST DEVICE
  Pixel(byte r, byte g, byte b)
  {
    set(r, g, b);
  }

  HOST DEVICE
  Pixel(const Color& c)
  {
    set(c);
  }

  HOST DEVICE
  void set(byte r, byte g, byte b)
  {
    this->r = r;
    this->g = g;
    this->b = b;
  }

  HOST DEVICE
  void set(const Color& c)
  {
    r = (byte)(255 * c.r);
    g = (byte)(255 * c.g);
    b = (byte)(255 * c.b);
  }

  HOST DEVICE
  Pixel& operator +=(const Pixel& p)
  {
    r += p.r;
    g += p.g;
    b += p.b;
    return *this;
  }

  HOST DEVICE
  Pixel& operator +=(const Color& c)
  {
    r += (byte)(255 * c.r);
    g += (byte)(255 * c.g);
    b += (byte)(255 * c.b);
    return *this;
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

  auto width() const
  {
    return _W;
  }

  auto height() const
  {
    return _H;
  }

  const Pixel* data() const
  {
    return _data;
  }

  const Pixel& operator ()(int x, int y) const
  {
#ifdef _DEBUG
    if (x < 0 || x >= _W || y < 0 || y >= _H)
      image_index_out_of_range();
#endif // _DEBUG
    return _data[y * _W + x];
  }

  Pixel& operator ()(int x, int y)
  {
#ifdef _DEBUG
    if (x < 0 || x >= _W || y < 0 || y >= _H)
      image_index_out_of_range();
#endif // _DEBUG
    return _data[y * _W + x];
  }

  auto length() const
  {
    return _W * _H;
  }

  const Pixel& operator [](int i) const
  {
#ifdef _DEBUG
    if (i < 0 || i >= _W * _H)
      image_index_out_of_range();
#endif // _DEBUG
    return _data[i];
  }

  Pixel& operator [](int i)
  {
#ifdef _DEBUG
    if (i < 0 || i >= _W * _H)
      image_index_out_of_range();
#endif // _DEBUG
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
  auto width() const
  {
    return _W;
  }

  auto height() const
  {
    return _H;
  }

  void setData(int x, int y, const ImageBuffer& buffer);

  void setData(const ImageBuffer& buffer)
  {
    setData(0, 0, buffer);
  }

  ImageBuffer data(int x, int y, int w, int h) const;

  ImageBuffer data() const
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
