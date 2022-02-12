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
// OVERVIEW: GLTextureFramebuffer.cpp
// ========
// Source file for OpenGL texture FBO.
//
// Author: Paulo Pagliosa
// Last revision: 31/01/2022

#include "graphics/GLTextureFramebuffer.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// GLTextureFramebuffer implementation
// ====================
GLTextureFramebuffer::~GLTextureFramebuffer()
{
  glDeleteTextures(1, &_texture);
  glDeleteRenderbuffers(1, &_depthBuffer);
  glDeleteFramebuffers(1, &_fbo);
}

GLTextureFramebuffer::GLTextureFramebuffer(int width, int height):
  _width{(uint32_t)width}, _height{(uint32_t)height}
{
  int textureUnit, texture;

  glGetIntegerv(GL_ACTIVE_TEXTURE, &textureUnit);
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture);
  glGenFramebuffers(1, &_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &_texture);
  glBindTexture(GL_TEXTURE_2D, _texture);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, width, height);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glGenRenderbuffers(1, &_depthBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, _depthBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
  glFramebufferTexture2D(GL_FRAMEBUFFER,
    GL_COLOR_ATTACHMENT0,
    GL_TEXTURE_2D,
    _texture,
    0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER,
    GL_DEPTH_ATTACHMENT,
    GL_RENDERBUFFER,
    _depthBuffer);

  const GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};

  glDrawBuffers(1, drawBuffers);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glActiveTexture(textureUnit);
  glBindTexture(GL_TEXTURE_2D, texture);
}

void
GLTextureFramebuffer::use()
{
  if (!_inUse)
  {
    glGetIntegerv(GL_VIEWPORT, _viewport);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    glViewport(0, 0, _width, _height);
    _inUse = true;
  }
}

void
GLTextureFramebuffer::disuse()
{
  if (_inUse)
  {
    glViewport(_viewport[0], _viewport[1], _viewport[2], _viewport[3]);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    _inUse = false;
  }
}

} // end namespace cg
