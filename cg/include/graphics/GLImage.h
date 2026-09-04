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
// OVERVIEW: GLImage.h
// ========
// Class definition for OpenGL image.
//
// Author: Paulo Pagliosa
// Last revision: 04/09/2026

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
  GLImage(int w, int h);

  // Destructor.
  ~GLImage() override;

  // Draws this image.
  void draw(int x, int y) const override;

  void bind() const;

  [[nodiscard]] operator uint32_t() const
  {
    return _handle;
  }

  // Creates an RGB texture.
  [[nodiscard]] static uint32_t createTexture(int w, int h);

  // Draws an RGB texture.
  static void drawTexture(uint32_t id, int x, int y, int w, int h);

private:
  class Drawer;

  uint32_t _handle;

  void setSubImage(int, int, int, int, const Pixel*) override;
  void getSubImage(int, int, int, int, Pixel*) const override;

  static Drawer* drawer();

}; // GLImage

} // end namespace cg

#endif // __GLImage_h
