//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2022, 2023 Paulo Pagliosa.                        |
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
// OVERVIEW: SceneWindowBase.h
// ========
// Class definition for scene window base.
//
// Author: Paulo Pagliosa
// Last revision: 01/08/2023

#ifndef __SceneWindowBase_h
#define __SceneWindowBase_h

#include "graphics/GLWindow.h"
#include "graphics/GLTextureFrameBuffer.h"
#include "graphics/SceneEditor.h"

namespace cg
{ // begin namespace


/////////////////////////////////////////////////////////////////////
//
// SceneWindowBase: scene window base class
// ===============
class SceneWindowBase: public GLWindow
{
public:
  GLRenderer* renderer() const
  {
    return _editor;
  }

protected:
  Color _selectedWireframeColor[2]
  {
    Color{255, 102, 0}, // parent
    Color{98, 119, 155, 64} // children
  };
  bool _showEditorView{true};
  bool _showPreview{true};

  using GLWindow::GLWindow;

  SceneEditor* editor() const
  {
    return _editor;
  }

  void initialize() override;
  void render() override;

  virtual void newScene();
  virtual void beginInitialize();
  virtual void endInitialize();
  virtual void initializeScene();
  virtual void drawAttachments();
  virtual bool onResize(int, int);
  virtual bool onMouseLeftPress(int, int);
  virtual bool onKeyPress(int, int);

  void editorView();
  void preview(Camera&);
  bool showErrorMessage(const char*) const;

  Material* createMaterial();
  Ray3f makeRay(int, int) const;

  static void inspectCamera(Camera&);
  static void inspectLight(Light&);
  static void inspectMaterial(Material&);

private:
  enum class MoveBits
  {
    Left = 1,
    Right = 2,
    Forward = 4,
    Back = 8,
    Up = 16,
    Down = 32
  };

  enum class DragBits
  {
    Rotate = 1,
    Pan = 2
  };

  Reference<SceneEditor> _editor;
  Reference<GLTextureFramebuffer> _fbo;
  Flags<MoveBits> _moveFlags{};
  Flags<DragBits> _dragFlags{};

  struct
  {
    int px, py;
    int cx, cy;

  } _mouse;

  bool windowResizeEvent(int, int) override;
  bool scrollEvent(double, double) override;
  bool mouseButtonInputEvent(int, int, int) override;
  bool mouseMoveEvent(double, double) override;
  bool keyInputEvent(int, int, int) override;

  virtual SceneBase* makeScene() = 0;

}; // SceneWindowBase

} // end namespace cg

namespace ImGui
{ // begin namespace ImGui

using namespace cg;

void objectNameInput(NameableObject& object);
void inputText(const char* label, const char* text);

inline bool
dragVec2(const char* label, vec2f& v, float min = 0, float max = 0)
{
  return ImGui::DragFloat2(label, &v.x, 0.01f, min, max, "%.2g");
}

inline bool
dragVec2(const char* label, vec2f& v, vec2f r)
{
  return ImGui::SliderFloat2(label, &v.x, r.x, r.y, "%.2g");
}

inline bool
dragVec3(const char* label, vec3f& v, float min = 0, float max = 0)
{
  // TODO: replace 0.1f
  return ImGui::DragFloat3(label, &v.x, 0.1f, min, max, "%.2g");
}

inline bool
dragVec3(const char* label, vec3f& v, vec2f r)
{
  return ImGui::SliderFloat3(label, &v.x, r.x, r.y, "%.2g");
}

inline bool
colorEdit3(const char* label, Color& color)
{
  return ImGui::ColorEdit3(label, &color.r);
}

bool showStyleSelector(const char* label);
void tooltip(const char* msg);

inline void
sectionLabel(const char* label)
{
  TextColored({1, 153.0f / 255, 51.0f / 255, 1}, "%s", label);
}

} // end namespace ImGui

#endif // __SceneWindodBase_h
