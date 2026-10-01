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
// OVERVIEW: GLTextureHelper.h
// ========
// Class definition for OpenGL texture helper.
//
// Author: Paulo Pagliosa
// Last revision: 30/09/2026

#ifndef __GLTextureHelper_h
#define __GLTextureHelper_h

#ifdef __APPLE__
#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif
#elif _WIN32
#define NOMINMAX
#endif
#include <GL/gl3w.h>

namespace cg
{ // begin namespace cg

inline void
allocateRGBTexture(GLenum target, int w, int h)
{
#ifndef __APPLE__
  glTexStorage2D(target, 1, GL_RGB8, w, h);
#else
  // macOS (OpenGL 4.1 fallback)
  glTexImage2D(target,
    0,
    GL_RGB8,
    w,
    h, 
    0,
    GL_RGB,
    GL_UNSIGNED_BYTE,
    nullptr
  );
  glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, 0);
#endif
}

inline GLuint
createRGBTexture(int w, int h)
{
  GLuint id;

  // Create texture
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  // Initialize texture
  allocateRGBTexture(GL_TEXTURE_2D, w, h);
  // Set texture sampler parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  return id;
}

} // end namespace cg

#endif // __GLTextureHelper_h
