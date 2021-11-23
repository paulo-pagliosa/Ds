//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018 Orthrus Group.                               |
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
// OVERVIEW: GLImage.h
// ========
// Class definition for OpenGL image.
//
// Author: Paulo Pagliosa
// Last revision: 21/11/2018

#ifndef __GLImage_h
#define __GLImage_h

#include "graphics/GLProgram.h"
#include "graphics/Image.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// GLImage: OpenGL image class
// =======
class GLImage: public Image
{
public:
  // Constructs an empty image.
  GLImage(int width, int height);

  // Destructor.
  ~GLImage() override;

  // Draws this image.
  void draw(int x, int y) const override;

  void bind() const;

  operator uint32_t() const
  {
    return _handle;
  }

  // Draws a texture.
  static void draw(uint32_t texture, int x, int y, int width, int height);

private:
  class Drawer;

  uint32_t _handle;

  void setSubImage(int, int, int, int, const Pixel*) override;
  void getSubImage(int, int, int, int, Pixel*) const override;

  static Drawer* drawer();

}; // GLImage

} // end namespace cg

#endif // __GLImage_h
