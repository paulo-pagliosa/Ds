//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2019 Orthrus Group.                         |
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
// OVERVIEW: Image.cpp
// ========
// Source file for generic image.
//
// Author: Paulo Pagliosa
// Last revision: 28/05/2019

#include "graphics/Image.h"
#include <algorithm>

namespace cg
{ // begin namespace cg

static inline void
image_bad_size()
{
  throw std::logic_error("Image: bad size");
}


/////////////////////////////////////////////////////////////////////
//
// ImageBuffer implementation
// ===========
ImageBuffer::ImageBuffer(int w, int h)
{
#ifdef _DEBUG
  if (w < 1 || h < 1)
    image_bad_size();
#endif // _DEBUG
  _W = w;
  _H = h;
  _data = new Pixel[(size_t)w * h];
}

ImageBuffer::ImageBuffer(ImageBuffer&& other) noexcept:
  _W{other._W},
  _H{other._H},
  _data{other._data}
{
  other._W = other._H = 0;
  other._data = nullptr;
}

ImageBuffer&
ImageBuffer::operator =(ImageBuffer&& other) noexcept
{
  delete[]_data;
  _W = other._W;
  _H = other._H;
  _data = other._data;
  other._W = other._H = 0;
  other._data = nullptr;
  return *this;
}


/////////////////////////////////////////////////////////////////////
//
// Image implementation
// =====
Image::Image(int w, int h):
  _W{w},
  _H{h}
{
#ifdef _DEBUG
  if (w < 1 || h < 1)
    image_bad_size();
#endif // _DEBUG
}

void
Image::setData(int x, int y, const ImageBuffer& buffer)
{
#ifdef _DEBUG
  if (x < 0 || x >= _W || y < 0 || y >= _H)
    image_index_out_of_range();
#endif // _DEBUG

  auto w = std::min(_W, buffer._W);
  auto h = std::min(_H, buffer._H);

  if (x + w > _W)
    w = _W - x;
  if (y + h > _H)
    h = _W - y;
  setSubImage(x, y, w, h, buffer._data);
}

ImageBuffer
Image::data(int x, int y, int w, int h) const
{
#ifdef _DEBUG
  if (x < 0 || x >= _W || y < 0 || y >= _H)
    image_index_out_of_range();
#endif // _DEBUG
  w = std::min(_W, w);
  h = std::min(_H, h);
  if (x + w > _W)
    w = _W - x;
  if (y + h > _H)
    h = _W - y;

  ImageBuffer buffer{w, h};

  getSubImage(x, y, w, h, buffer._data);
  return buffer;
}

} // end namespace cg
