//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019, 2023 Paulo Pagliosa.                        |
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
// OVERVIEW: GLFramebuffer.h
// ========
// Class definition for OpenGL FBO.
//
// Author: Paulo Pagliosa
// Last revision: 30/07/2023

#ifndef __GLFramebuffer_h
#define __GLFramebuffer_h

#include "graphics/GLBuffer.h"

namespace cg
{ // begin namespace cg

class GLFramebuffer;

namespace fbo
{ // begin namespace fbo

static constexpr auto MaxColors = 7;

enum class ColorAttachment: GLenum
{
  Color0 = GL_COLOR_ATTACHMENT0,
  Color1 = GL_COLOR_ATTACHMENT1,
  Color2 = GL_COLOR_ATTACHMENT2,
  Color3 = GL_COLOR_ATTACHMENT3,
  Color4 = GL_COLOR_ATTACHMENT4,
  Color5 = GL_COLOR_ATTACHMENT5,
  Color6 = GL_COLOR_ATTACHMENT6
};

class Description
{
public:
  Description(uint32_t width, uint32_t height):
    width{width}, height{height}
  {
    // do nothing
  }

  auto& attachDepthBuffer(GLenum format = GL_DEPTH_COMPONENT24)
  {
    if (depthBufferFormat == 0)
      depthBufferFormat = format;
    return *this;
  }

  auto& attachColorBuffer(ColorAttachment attachment, GLenum format = GL_RGB8)
  {
    if (colorCount < MaxColors)
    {
      attachments[colorCount] = attachment;
      formats[colorCount++] = format;
    }
    return *this;
  }

private:
  uint32_t width;
  uint32_t height;
  uint32_t colorCount{};
  GLenum depthBufferFormat{};
  ColorAttachment attachments[MaxColors];
  GLenum formats[MaxColors];

  friend GLFramebuffer;

}; // Description

} // end namesapce fbo


/////////////////////////////////////////////////////////////////////
//
// GLFramebuffer: OpenGL FBO class
// =============
class GLFramebuffer: public SharedObject
{
public:
  ~GLFramebuffer() override;

  static GLFramebuffer* New(const fbo::Description& description);

  auto width() const
  {
    return _description.width;
  }

  auto height() const
  {
    return _description.height;
  }

  void beginRead(int index) const;
  void endRead() const;

  bool resize(uint32_t width, uint32_t height);

  void beginDraw();
  void endDraw();

  void copyToFramebuffer(int index, int x, int y, int w, int h);

private:
  GLuint _fbo;
  GLuint _depthBuffer{};
  GLuint _colorBuffers[fbo::MaxColors]{};
  fbo::Description _description;
  mutable struct
  {
    uint8_t read : 1;
    uint8_t draw : 1;

  } _flags{};

  GLFramebuffer(const fbo::Description&);

}; // GLFramebuffer

namespace fbo
{ // begin namespace fbo

class ReadBufferBase
{
public:
  ~ReadBufferBase()
  {
    if (_size > 16)
      delete []_data;
  }

protected:
  ReadBufferBase(const GLFramebuffer& fbo, uint32_t size):
    _fbo{&fbo}
  {
    _data = size > 16 ? (buffer = new uint8_t[size]) : bytes;
    _size = size;
  }

  const void* read(int index,
    int x,
    int y,
    int w,
    int h,
    GLenum format,
    GLenum type) const;

private:
  union
  {
    uint8_t bytes[16];
    uint8_t* buffer;
  };
  uint8_t* _data;
  const GLFramebuffer* _fbo;
  uint32_t _size;

}; // ReadBufferBase

template <typename>
inline constexpr bool dependent_false_v = false;

template <typename T>
class ReadBuffer: public ReadBufferBase
{
public:
  ReadBuffer(const GLFramebuffer& fbo, uint32_t count):
    ReadBufferBase{fbo, count * sizeof(T)}
  {
    // do nothing
  }

#define READ(type) (T*)ReadBufferBase::read(index, x, y, w, h, format, type)

  const T* read(int index, int x, int y, int w, int h, GLenum format)
  {
    if constexpr (std::is_same_v<T, char>)
      return READ(GL_BYTE);
    else if constexpr (std::is_same_v<T, unsigned char>)
      return READ(GL_UNSIGNED_BYTE);
    else if constexpr (std::is_same_v<T, short>)
      return READ(GL_SHORT);
    else if constexpr (std::is_same_v<T, unsigned short>)
      return READ(GL_UNSIGNED_SHORT);
    else if constexpr (std::is_same_v<T, int>)
      return READ(GL_INT);
    else if constexpr (std::is_same_v<T, unsigned int>)
      return READ(GL_UNSIGNED_INT);
    else if constexpr (std::is_same_v<T, float>)
      return READ(GL_FLOAT);
    else
    {
      // See dependent_false_v<T> workaround in
      // https://en.cppreference.com/w/cpp/language/if
      static_assert(dependent_false_v<T>, "Invalid FBO read buffer type");
      return nullptr;
    }
  }

#undef READ

}; // ReadBuffer

} // end namespace fbo

} // end namespace cg

#endif // __GLFramebuffer_h
