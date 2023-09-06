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
// OVERVIEW: GLWindow.h
// ========
// Class definition for OpenGL window.
//
// Author: Paulo Pagliosa
// Last revision: 05/09/2023

#ifndef __GLWindow_h
#define __GLWindow_h

#include "graphics/Color.h"
#include "graphics/GLProgram.h"
#include "imgui.h"

namespace cg
{ // begin namespace cg

class Application;


/////////////////////////////////////////////////////////////////////
//
// GLWindow: OpenGL windows class
// ========
class GLWindow
{
public:
  /// Destructor.
  virtual ~GLWindow();

  /// Returns the title of this window.
  auto title() const
  {
    return _title;
  }

  /// Returns the width of this window.
  auto width() const
  {
    return _width;
  }

  /// Returns the height of this window.
  auto height() const
  {
    return _height;
  }

protected:
  Color backgroundColor{Color::gray};

  /// Constructs an intance of GLWindow.
  GLWindow(const char* title, int width, int height);

  auto argc() const
  {
    return _argc;
  }

  auto argv(int i) const
  {
    assert(i >= 0 && i < _argc);
    return _argv[i];
  }

  // Event handlers.
  virtual bool cursorEnterWindowEvent(int entered);
  virtual bool mouseMoveEvent(double xPos, double yPos);
  virtual bool mouseButtonInputEvent(int button, int action, int mods);
  virtual bool scrollEvent(double xOffset, double yOffset);
  virtual bool windowResizeEvent(int width, int height);
  virtual bool keyInputEvent(int key, int action, int mods);

  /// Initializes this window.
  virtual void initialize();

  /// Updates the scene associated with this window.
  virtual void update();

  /// Updates the GUI of this window.
  virtual void gui();

  /// Renders the scene associated with this window.
  virtual void render();

  /// Terminates this window.
  virtual void terminate();

  /// Clears the drawing on this window.
  void clear(const Color& color);

  void clear()
  {
    clear(backgroundColor);
  }

  /// Returns the elapsed time since the last frame.
  auto deltaTime() const
  {
    return _deltaTime;
  }

  /// Returns the cursor position on this window.
  void cursorPosition(int& x, int& y) const
  {
    double xpos;
    double ypos;

    glfwGetCursorPos(_window, &xpos, &ypos);
    x = (int)xpos;
    y = (int)ypos;
  }

  /// Returns true if \c key is pressed.
  bool isKeyPressed(int key) const
  {
    return glfwGetKey(_window, key) == GLFW_PRESS;
  }

  void shutdown()
  {
    glfwSetWindowShouldClose(_window, GL_TRUE);
  }

private:
  GLFWwindow* _window{};
  char** _argv;
  int _argc;
  int _width;
  int _height;
  float _deltaTime{};
  std::string _title;
  bool _paused{};

  void registerGlfwCallBacks();
  void centerWindow();
  void mainLoop();
  void show(int, char**);

  static void cursorEnterWindowCallBack(GLFWwindow*, int);
  static void mouseMoveCallBack(GLFWwindow*, double, double);
  static void mouseButtonCallBack(GLFWwindow*, int, int, int);
  static void scrollCallBack(GLFWwindow*, double, double);
  static void windowResizeCallBack(GLFWwindow*, int, int);
  static void keyInputCallBack(GLFWwindow*, int, int, int, int);

  friend Application;

}; // GLWindow

} // end namespace cg

#endif // __GLWindow_h
