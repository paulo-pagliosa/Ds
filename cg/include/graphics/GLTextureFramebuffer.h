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
// OVERVIEW: GLTextureFramebuffer.h
// ========
// Class definition for OpenGL texture FBO.
//
// Author: Paulo Pagliosa
// Last revision: 10/08/2022

#ifndef __GLTextureFramebuffer_h
#define __GLTextureFramebuffer_h

#include "graphics/GLBuffer.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// GLTextureFramebuffer: OpenGL texture FBO class
// ====================
class GLTextureFramebuffer: public SharedObject
{
public:
  ~GLTextureFramebuffer() override;

  GLTextureFramebuffer(int width, int height);

  auto width() const
  {
    return (int)_width;
  }

  auto height() const
  {
    return (int)_height;
  }

  uint32_t texture() const
  {
    return _texture;
  }

  void use();
  void disuse();

private:
  uint32_t _width;
  uint32_t _height;
  bool _inUse{};
  GLuint _fbo;
  GLuint _texture;
  GLuint _depthBuffer;
  int _viewport[4];

}; // GLTextureFramebuffer

} // end namespace cg

#endif // __GLTextureFramebuffer_h
