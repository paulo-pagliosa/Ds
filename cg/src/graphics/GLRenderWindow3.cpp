//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2020, 2023 Paulo Pagliosa.                        |
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
// OVERVIEW: GLRenderWindow3.cpp
// ========
// Source file for OpenGL 3D render window.
//
// Author: Paulo Pagliosa
// Last revision: 30/07/2023

#include "graphics/GLRenderWindow3.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// GLRenderWindow3 implementation
// ===============
void
GLRenderWindow3::initialize()
{
  _camera = new Camera{};
  _camera->setAspectRatio((float)width() / (float)height());
  _g3 = makeRenderer(*_camera);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0f, 1.0f);
  glEnable(GL_LINE_SMOOTH);
}

void
GLRenderWindow3::render()
{
  GLWindow::render();
  _g3->updateView(*_camera);
  renderScene();
}

GLGraphics3*
GLRenderWindow3::makeRenderer(Camera&)
{
  return new GLGraphics3{};
}

void
GLRenderWindow3::renderScene()
{
  // do nothing
}

bool
GLRenderWindow3::windowResizeEvent(int width, int height)
{
  _camera->setAspectRatio((float)width / (float)height);
  return GLWindow::windowResizeEvent(width, height);
}

namespace
{

constexpr auto CAMERA_RES = 0.01f;
constexpr auto ZOOM_SCALE = 1.01f;

}

bool
GLRenderWindow3::keyInputEvent(int key, int action, int)
{
  if (ImGui::GetIO().WantCaptureKeyboard || action == GLFW_RELEASE)
    return false;

  const auto delta = _camera->distance() * CAMERA_RES;
  auto d = vec3f::null();

  switch (key)
  {
    case GLFW_KEY_W:
      d.z -= delta;
      break;
    case GLFW_KEY_S:
      d.z += delta;
      break;
    case GLFW_KEY_A:
      d.x -= delta;
      break;
    case GLFW_KEY_D:
      d.x += delta;
      break;
    case GLFW_KEY_Q:
      d.y += delta;
      break;
    case GLFW_KEY_Z:
      d.y -= delta;
      break;
    default:
      return false;
  }
  _camera->translate(d);
  return true;
}

bool
GLRenderWindow3::scrollEvent(double, double yOffset)
{
  if (ImGui::GetIO().WantCaptureMouse)
    return false;
  _camera->zoom(yOffset < 0 ? 1.0f / ZOOM_SCALE : ZOOM_SCALE);
  return true;
}

bool
GLRenderWindow3::mouseButtonInputEvent(int button, int actions, int)
{
  if (ImGui::GetIO().WantCaptureMouse)
    return false;

  auto active = actions == GLFW_PRESS;

  cursorPosition(_pivotX, _pivotY);
  if (button == GLFW_MOUSE_BUTTON_RIGHT)
    _dragFlags.enable(DragBits::Rotate, active);
  else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    _dragFlags.enable(DragBits::Pan, active);
  else if (button == GLFW_MOUSE_BUTTON_LEFT)
    if (active)
      return onMouseLeftPress(_pivotX, _pivotY);
  return true;
}

bool
GLRenderWindow3::mouseMoveEvent(double xPos, double yPos)
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
      const auto da = -_camera->viewAngle() * CAMERA_RES;
      _camera->rotateYX(dx * da, dy * da, isKeyPressed(GLFW_KEY_LEFT_ALT));
    }
    else if (_dragFlags.isSet(DragBits::Pan))
    {
      const auto dt = -_camera->distance() * CAMERA_RES;
      _camera->translate(-dt * math::sign(dx), dt * math::sign(dy), 0);
    }
  }
  return true;
}

bool
GLRenderWindow3::onMouseLeftPress(int, int)
{
  return false;
}

} // end namespace cg
