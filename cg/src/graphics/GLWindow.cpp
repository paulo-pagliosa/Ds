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
// OVERVIEW: GLWindow.cpp
// ========
// Source file for OpenGL window.
//
// Author: Paulo Pagliosa
// Last revision: 05/09/2023

#include "core/Exception.h"
#include "graphics/Application.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// GLWindow implementation
// ========
GLWindow::GLWindow(const char* title, int width, int height):
  _width{width}, _height{height},
  _title{title},
  _argc{},
  _argv{}
{
  // do nothing
}

GLWindow::~GLWindow()
{
  if (_window != nullptr)
    glfwDestroyWindow(_window);
}

bool
GLWindow::cursorEnterWindowEvent(int entered)
{
  (void)entered;
  return false;
}

bool
GLWindow::mouseMoveEvent(double xPos, double yPos)
{
  (void)xPos;
  (void)yPos;
  return false;
}

bool
GLWindow::mouseButtonInputEvent(int button, int action, int mods)
{
  (void)button;
  (void)action;
  (void)mods;
  return false;
}

bool
GLWindow::scrollEvent(double xOffset, double yOffset)
{
  (void)xOffset;
  (void)yOffset;
  return false;
}

bool
GLWindow::windowResizeEvent(int width, int height)
{
  glViewport(0, 0, width, height);
  return true;
}

bool
GLWindow::keyInputEvent(int key, int action, int mods)
{
  (void)key;
  (void)action;
  (void)mods;
  return false;
}

void
GLWindow::initialize()
{
  // do nothing
}

void
GLWindow::update()
{
  // do nothing
}

void
GLWindow::gui()
{
  // do nothing
}

void
GLWindow::render()
{
  clear();
}

void
GLWindow::terminate()
{
  // do nothing
}

void
GLWindow::clear(const Color& color)
{
  glClearColor(color.r, color.g, color.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void
GLWindow::registerGlfwCallBacks()
{
  glfwSetKeyCallback(_window, keyInputCallBack);
  glfwSetWindowSizeCallback(_window, windowResizeCallBack);
  glfwSetScrollCallback(_window, scrollCallBack);
  glfwSetMouseButtonCallback(_window, mouseButtonCallBack);
  glfwSetCursorPosCallback(_window, mouseMoveCallBack);
  glfwSetCursorEnterCallback(_window, cursorEnterWindowCallBack);
  glfwSetCharCallback(_window, ImGui_ImplGlfw_CharCallback);
}

void
GLWindow::centerWindow()
{
  auto monitor = glfwGetWindowMonitor(_window);

  if (monitor == nullptr)
    monitor = glfwGetPrimaryMonitor();
  glfwGetWindowSize(_window, &_width, &_height);

  const auto v = glfwGetVideoMode(monitor);
  const auto x = (v->width - _width) >> 1;
  const auto y = (v->height - _height) >> 1;

  glfwSetWindowPos(_window, x, y);
}

inline void
GLWindow::mainLoop()
{
  while (!glfwWindowShouldClose(_window))
  {
    // Pool and handle events.
    glfwPollEvents();
    // Start the Dear ImGui frame.
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    _deltaTime = 1000.0f / ImGui::GetIO().Framerate;
    if (!_paused)
      // Update the scene.
      update();
    else
    {
      ImGui::OpenPopup("Paused");
      if (ImGui::BeginPopupModal("Paused",
        nullptr,
        ImGuiWindowFlags_NoTitleBar))
      {
        ImGui::Text("Application '%s' is paused.", _title.c_str());
        ImGui::Separator();
        if (ImGui::Button("Continue"))
        {
          _paused = false;
          ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Exit"))
          shutdown();
        ImGui::EndPopup();
      }
    }
    // Update the GUI.
    gui();
    // Render the scene.
    render();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(_window);
  }
}

inline auto
createGlfwWindow(const char* title, int width, int height)
{
  return glfwCreateWindow(width, height, title, nullptr, nullptr);
}

void
GLWindow::show(int argc, char** argv)
{
  // Create the GLFW window.
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 8);
  _window = createGlfwWindow(_title.c_str(), _width, _height);
  if (_window == nullptr)
    runtimeError("Unable to create GLFW window");
  glfwSetWindowUserPointer(_window, this);
  centerWindow();
  glfwMakeContextCurrent(_window);
  glfwSwapInterval(1);
  gl3wInit();
  if (!gl3wIsSupported(4, 0))
    runtimeError("OpenGL v400 is not supported");
  glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  registerGlfwCallBacks();
  // Setup Dear ImGui binding.
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  // Setup style.
  auto& style = ImGui::GetStyle();

  style.FrameRounding = style.WindowRounding = 0.0f;
  style.FrameBorderSize = 1.0f;
  style.Alpha = 0.9f;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(_window, false);
  ImGui_ImplOpenGL3_Init("#version 400");

  // Clear error buffer.
  while (glGetError() != GL_NO_ERROR)
    ;
  _argc = argc;
  _argv = argv;
  // Initialize the app.
  initialize();
  // Poll and handle user events.
  mainLoop();
  // Terminate the app.
  terminate();
}

inline auto
getWindow(GLFWwindow* window)
{
  return static_cast<GLWindow*>(glfwGetWindowUserPointer(window));
}

void
GLWindow::windowResizeCallBack(GLFWwindow* window, int width, int height)
{
  auto self = getWindow(window);
  
  self->_width = width;
  self->_height = height;
  self->windowResizeEvent(width, height);
}

void
GLWindow::keyInputCallBack(GLFWwindow* window,
  int key,
  int scancode,
  int action,
  int mods)
{
  auto self = getWindow(window);

  if (action == GLFW_PRESS)
  {
    if (mods == GLFW_MOD_ALT && key == GLFW_KEY_F4)
    {
      self->shutdown();
      return;
    }
    if (mods == GLFW_MOD_CONTROL && key == GLFW_KEY_P && !self->_paused)
    {
      self->_paused = true;
      return;
    }
  }
  if (!self->keyInputEvent(key, action, mods))
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}

void
GLWindow::scrollCallBack(GLFWwindow* window, double xOffSet, double yOffSet)
{
  getWindow(window)->scrollEvent(xOffSet, yOffSet);
  ImGui_ImplGlfw_ScrollCallback(window, xOffSet, yOffSet);
}

void
GLWindow::mouseButtonCallBack(GLFWwindow* window,
  int button,
  int action,
  int mods)
{
  getWindow(window)->mouseButtonInputEvent(button, action, mods);
  ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}

void
GLWindow::mouseMoveCallBack(GLFWwindow* window, double xPos, double yPos)
{
  getWindow(window)->mouseMoveEvent(xPos, yPos);
}

void
GLWindow::cursorEnterWindowCallBack(GLFWwindow* window, int entered)
{
  getWindow(window)->cursorEnterWindowEvent(entered);
}

} // end namespace cg
