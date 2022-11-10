//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2022 Paulo Pagliosa.                        |
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
// OVERVIEW: GLImage.cpp
// ========
// Source file for OpenGL image.
//
// Author: Paulo Pagliosa
// Last revision: 10/11/2022

#include "graphics/GLImage.h"
#include <memory>

namespace cg
{ // begin namespace cg

#define STRINGIFY(A) "#version 400\n"#A

static const char* vertexShader = STRINGIFY(
  uniform vec4 vp[] = vec4[4](
    vec4(-1.0f, -1.0f, 0, 1),
    vec4(+1.0f, -1.0f, 0, 1),
    vec4(+1.0f, +1.0f, 0, 1),
    vec4(-1.0f, +1.0f, 0, 1));
  uniform vec2 uv[] = vec2[4](
    vec2(0.0f, 0.0f),
    vec2(1.0f, 0.0f),
    vec2(1.0f, 1.0f),
    vec2(0.0f, 1.0f));
  out vec2 v_uv;
  void main()
  {
    gl_Position = vp[gl_VertexID];
    v_uv = uv[gl_VertexID];
  }
);

static const char* fragmentShader = STRINGIFY(
  uniform sampler2D tex;
  in vec2 v_uv;
  out vec4 f_color;
  void main()
  {
    f_color = texture(tex, v_uv);
  }
);

//
// Auxiliary functions
//
inline GLuint
createRGBTexture(int w, int h)
{
  GLuint id;

  // Create texture
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  // Initialize texture
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, w, h);
  // Set texture sampler parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  return id;
}

inline void
setTextureData(int x, int y, int w, int h, const Pixel* data)
{
  glTexSubImage2D(GL_TEXTURE_2D,
    0,
    x,
    y,
    w,
    h,
    GL_RGB,
    GL_UNSIGNED_BYTE,
    data);
}

inline void
getTextureData(int x, int y, int w, int h, Pixel* data)
{
  // TODO
}


/////////////////////////////////////////////////////////////////////
//
// GLImage::Drawer: GL image drawer class
// ===============
class GLImage::Drawer: public GLSL::Program
{
public:
  // Constructor.
  Drawer():
    GLSL::Program{"Texture Drawer"}
  {
    auto cp = GLSL::Program::current();

    setShaders(vertexShader, fragmentShader).use();
    glGenVertexArrays(1, &_vao);
    GLSL::Program::setCurrent(cp);
  }

  ~Drawer()
  {
    glDeleteVertexArrays(1, &_vao);
  }

  // Render an OpenGL texture.
  void draw(uint32_t texture, int x, int y, int width, int height)
  {
    auto dt = glIsEnabled(GL_DEPTH_TEST);
    auto cp = GLSL::Program::current();
    GLint cv[4];
    GLint cva;
    GLint ct;

    this->use();
    glGetIntegerv(GL_VIEWPORT, cv);
    glViewport(x, y, width, height);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &cva);
    glBindVertexArray(_vao);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &ct);
    glBindTexture(GL_TEXTURE_2D, texture);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    if (dt)
      glEnable(GL_DEPTH_TEST);
    glViewport(cv[0], cv[1], cv[2], cv[3]);
    glBindTexture(GL_TEXTURE_2D, ct);
    glBindVertexArray(cva);
    GLSL::Program::setCurrent(cp);
  }

  // Render a image.
  void draw(const GLImage& image, int x, int y)
  {
    draw(image, x, y, image.width(), image.height());
  }

private:
  GLuint _vao;

}; // GLImage::Drawer


/////////////////////////////////////////////////////////////////////
//
// GLImage implementation
// =======
GLImage::GLImage(int w, int h):
  Image{w, h},
  _handle{createTexture(w, h)}
{
  // do nothing
}

GLImage::~GLImage()
{
  glDeleteTextures(1, &_handle);
}

inline void
GLImage::bind() const
{
  glBindTexture(GL_TEXTURE_2D, _handle);
}

void
GLImage::setSubImage(int x, int y, int w, int h, const Pixel* data)
{
  setTextureData(x, y, w, h, data);
}

void
GLImage::getSubImage(int x, int y, int w, int h, Pixel* data) const
{
  getTextureData(x, y, w, h, data);
}

GLImage::Drawer*
GLImage::drawer()
{
  static Drawer* instance;

  if (instance == nullptr)
    instance = new Drawer{};
  return instance;
}

void
GLImage::draw(int x, int y) const
{
  drawer()->draw(*this, x, y);
}

uint32_t
GLImage::createTexture(int w, int h)
{
  return createRGBTexture(w, h);
}
void
GLImage::drawTexture(uint32_t id, int x, int y, int w, int h)
{
  drawer()->draw(id, x, y, w, h);
}

} // end namespace cg
