//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2023 Paulo Pagliosa.                        |
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
// OVERVIEW: GLRenderWindow2.cpp
// ========
// Source file for OpenGL 2D render window.
//
// Author: Paulo Pagliosa
// Last revision: 30/07/2023

#include "graphics/GLRenderWindow2.h"

namespace cg
{ // begin namespace cg


//////////////////////////////////////////////////////////
//
// GLRenderWindow2 implementation
// ===============
void
GLRenderWindow2::initialize()
{
  _g2 = new GLGraphics2();
  _g2->setAspectRatio(float(width()) / height());
}

namespace
{

constexpr auto MOVE_SCALE = 0.01f;
constexpr auto ZOOM_SCALE = 1.05f;

}

void
GLRenderWindow2::render()
{
  GLWindow::render();
  _g2->updateView();
  renderScene();
}

void
GLRenderWindow2::renderScene()
{
  // do nothing
}

bool
GLRenderWindow2::windowResizeEvent(int width, int height)
{
  _g2->setAspectRatio((float)width / height);
  return GLWindow::windowResizeEvent(width, height);
}

bool
GLRenderWindow2::keyInputEvent(int key, int action, int)
{
  if (ImGui::GetIO().WantCaptureKeyboard || action == GLFW_RELEASE)
    return false;

  const auto delta = _g2->bounds().size() * MOVE_SCALE;
  auto d = vec2f::null();

  switch (key)
  {
    case GLFW_KEY_A:
      d.x -= delta.x;
      break;
    case GLFW_KEY_D:
      d.x += delta.x;
      break;
    case GLFW_KEY_Q:
      d.y += delta.y;
      break;
    case GLFW_KEY_Z:
      d.y -= delta.y;
      break;
    default:
      return false;
  }
  _g2->pan(d.x, d.y);
  return true;
}

bool
GLRenderWindow2::scrollEvent(double, double yOffset)
{
  if (ImGui::GetIO().WantCaptureMouse)
    return false;
  _g2->zoom(yOffset < 0 ? 1.0f / ZOOM_SCALE : ZOOM_SCALE);
  return true;
}

bool
GLRenderWindow2::mouseButtonInputEvent(int button, int actions, int)
{
  if (ImGui::GetIO().WantCaptureMouse)
    return false;

  auto active = actions == GLFW_PRESS;

  cursorPosition(_pivotX, _pivotY);
  switch (button)
  {
    case GLFW_MOUSE_BUTTON_RIGHT:
      _dragFlags.enable(DragBits::Rotate, active);
      break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
      _dragFlags.enable(DragBits::Pan, active);
      break;
    case GLFW_MOUSE_BUTTON_LEFT:
      if (active)
        return onMouseLeftPress(_pivotX, _pivotY);
  }
  return true;
}

bool
GLRenderWindow2::mouseMoveEvent(double xPos, double yPos)
{
  if (!uint32_t(_dragFlags))
    return false;
  _mouseX = (int)xPos;
  _mouseY = (int)yPos;

  const auto dx = (_pivotX - _mouseX);
  const auto dy = (_pivotY - _mouseY);

  _pivotX = _mouseX;
  _pivotY = _mouseY;
  if (dx != 0 || dy != 0)
  {
    if (_dragFlags.isSet(DragBits::Rotate))
    {
      // TODO: rotate
    }
    else if (_dragFlags.isSet(DragBits::Pan))
    {
      const auto dt = _g2->bounds().size() * MOVE_SCALE;
      _g2->pan(dt.x * dx, -dt.y * dy);
    }
  }
  return true;
}

bool
GLRenderWindow2::onMouseLeftPress(int, int)
{
  return false;
}

} // end namespace cg
