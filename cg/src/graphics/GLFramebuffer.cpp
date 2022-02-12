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
// OVERVIEW: GLFramebuffer.cpp
// ========
// Source file for OpenGL FBO.
//
// Author: Paulo Pagliosa
// Last revision: 31/01/2022

#include "graphics/GLFramebuffer.h"
#include <cassert>

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// GLFramebuffer implementation
// =============
GLFramebuffer*
GLFramebuffer::New(const fbo::Description& description)
{
  auto fbo = new GLFramebuffer{description};
  auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

  if (status != GL_FRAMEBUFFER_COMPLETE)
  {
    delete fbo;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    throw std::runtime_error("FBO creation error");
  }
  return fbo;
}

GLFramebuffer::~GLFramebuffer()
{
  glDeleteRenderbuffers(_description.colorCount, _colorBuffers);
  if (_description.depthBufferFormat)
    glDeleteRenderbuffers(1, &_depthBuffer);
  glDeleteFramebuffers(1, &_fbo);
}

GLFramebuffer::GLFramebuffer(const fbo::Description& description):
  _description{description}
{
  if (_description.colorCount == 0)
    _description.attachColorBuffer(fbo::ColorAttachment::Color0);
  glGenFramebuffers(1, &_fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
  if (_description.depthBufferFormat)
  {
    glGenRenderbuffers(1, &_depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER,
      _description.depthBufferFormat,
      _description.width,
      _description.height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
      GL_DEPTH_ATTACHMENT,
      GL_RENDERBUFFER,
      _depthBuffer);
  }
  glGenRenderbuffers(_description.colorCount, _colorBuffers);
  for (uint32_t i = 0; i < _description.colorCount; ++i)
  {
    glBindRenderbuffer(GL_RENDERBUFFER, _colorBuffers[i]);
    glRenderbufferStorage(GL_RENDERBUFFER, _description.formats[i],
      _description.width,
      _description.height);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER,
      (GLenum)_description.attachments[i],
      GL_RENDERBUFFER,
      _colorBuffers[i]);
  }
  glDrawBuffers(_description.colorCount, (GLenum*)_description.attachments);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool
GLFramebuffer::resize(uint32_t width, uint32_t height)
{
  if (_description.width >= width && _description.height >= height)
    return false;
  _description.width = width;
  _description.height = height;
  if (_description.depthBufferFormat)
  {
    glBindRenderbuffer(GL_RENDERBUFFER, _depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER,
      _description.depthBufferFormat,
      width,
      height);
  }
  for (uint32_t i = 0; i < _description.colorCount; ++i)
  {
    glBindRenderbuffer(GL_RENDERBUFFER, _colorBuffers[i]);
    glRenderbufferStorage(GL_RENDERBUFFER, _description.formats[i],
      width,
      height);
  }
  return true;
}

void
GLFramebuffer::beginRead(int index) const
{
  if (!_flags.read)
  {
    assert(index < (int)_description.colorCount);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, _fbo);
    _flags.read = true;
  }
  glReadBuffer((GLenum)_description.attachments[index]);
}

void
GLFramebuffer::endRead() const
{
  if (_flags.read)
  {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    _flags.read = false;
  }
}

void
GLFramebuffer::beginDraw()
{
  if (!_flags.draw)
  {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
    _flags.draw = true;
  }
}

void
GLFramebuffer::endDraw()
{
  if (_flags.draw)
  {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    _flags.draw = false;
  }
}

void
GLFramebuffer::copyToFramebuffer(int index, int x, int y, int w, int h)
{
  assert(x + w < (int)width());
  assert(y + h < (int)height());
  beginRead(index);
  glBlitFramebuffer(x, y, w, h, x, y, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
  endRead();
}

const void*
fbo::ReadBufferBase::read(int index,
  int x,
  int y,
  int w,
  int h,
  GLenum format,
  GLenum type) const
{
  assert(x + w < (int)_fbo->width());
  assert(y + h < (int)_fbo->height());
  _fbo->beginRead(index);
  glReadPixels(x, y, w, h, format, type, _data);
  _fbo->endRead();
  return _data;
}

} // end namespace cg
