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
// OVERVIEW: SceneWindowBase.cpp
// ========
// Source file for scene window base.
//
// Author: Paulo Pagliosa
// Last revision: 01/08/2023

#include "graphics/Assets.h"
#include "graphics/Renderer.h"
#include "graphics/SceneWindowBase.h"

namespace ImGui
{ // begin namespace ImGui

void
objectNameInput(cg::NameableObject& object)
{
  constexpr auto bufferSize = 64;
  static char buffer[bufferSize];
  static NameableObject* current;

  if (&object != current)
  {
    snprintf(buffer, bufferSize, "%s", object.name());
    current = &object;
  }
  if (InputText("Name", buffer, bufferSize))
    object.setName(buffer);
}

void
inputText(const char* label, const char* text)
{
  constexpr auto bufferSize = 64;
  char buffer[bufferSize];

  snprintf(buffer, bufferSize, "%s", text);
  InputText(label, buffer, bufferSize, ImGuiInputTextFlags_ReadOnly);
}

bool
showStyleSelector(const char* label)
{
  static int style = 1;

  if (!ImGui::Combo(label, &style, "Classic\0Dark\0Light\0"))
    return false;
  switch (style)
  {
    case 0: ImGui::StyleColorsClassic();
      break;
    case 1: ImGui::StyleColorsDark();
      break;
    case 2: ImGui::StyleColorsLight();
      break;
  }
  return true;
}

void
tooltip(const char* msg)
{
  TextDisabled("(?)");
  if (IsItemHovered())
  {
    BeginTooltip();
    PushTextWrapPos(GetFontSize() * 35.0f);
    TextUnformatted(msg);
    PopTextWrapPos();
    EndTooltip();
  }
}

} // end namespace ImGui

namespace cg
{ // begin namespace


/////////////////////////////////////////////////////////////////////
//
// SceneWindowBase implementation
// ===============
void
SceneWindowBase::initialize()
{
  beginInitialize();

  auto scene = makeScene();

  assert(scene != nullptr);
  _editor = new SceneEditor{*scene};

  auto w = width(), h = height();

  _editor->setImageSize(w, h);
  _editor->setDefaultView((float)w / (float)h);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0f, 1.0f);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_MULTISAMPLE);
  endInitialize();
  initializeScene();
}

void
SceneWindowBase::render()
{
  _editor->render();
  drawAttachments();
  _editor->drawGround();
}

void
SceneWindowBase::newScene()
{
  auto scene = makeScene();

  assert(scene != nullptr);
  _editor->setScene(*scene);
  initializeScene();
}

void
SceneWindowBase::beginInitialize()
{
  // do nothing
}

void
SceneWindowBase::endInitialize()
{
  // do nothing
}

void
SceneWindowBase::initializeScene()
{
  // do nothing
}

void
SceneWindowBase::drawAttachments()
{
  // do nothing
}

bool
SceneWindowBase::onResize(int width, int height)
{
  (void)width;
  (void)height;
  return true;
}

bool
SceneWindowBase::onMouseLeftPress(int x, int y)
{
  (void)x;
  (void)y;
  return false;
}

bool
SceneWindowBase::onKeyPress(int key, int mods)
{
  (void)key;
  (void)mods;
  return false;
}

void
SceneWindowBase::editorView()
{
  if (!_showEditorView)
    return;
  ImGui::Begin("Editor View Settings");
  if (ImGui::Button("Set Default View"))
    _editor->setDefaultView(float(width()) / float(height()));
  ImGui::Separator();
  {
    auto camera = _editor->camera();
    vec3f temp;

    temp = camera->position();
    if (ImGui::dragVec3("Position", temp))
      camera->setPosition(temp);
    temp = camera->eulerAngles();
    if (ImGui::dragVec3("Rotation", temp))
      camera->setEulerAngles(temp);
    inspectCamera(*camera);
    ImGui::Separator();
  }
  {
    // TODO: shade modes
    static int sm;

    ImGui::Combo("Shading Mode",
      &sm,
      /*"None\0Flat\0Gouraud\0"*/"Phong\0""\0");

    // TODO: show/color edges
    static Color edgeColor;
    static bool showEdges;

    ImGui::colorEdit3("Edges", edgeColor);
    ImGui::SameLine();
    ImGui::Checkbox("###showEdges", &showEdges);
    _editor->renderMode = showEdges ?
      GLRenderer::RenderMode::HiddenLines :
      GLRenderer::RenderMode::Smooth;
  }
  ImGui::Separator();
  ImGui::Checkbox("Show Ground", &_editor->showGround);
  {
    auto gc = _editor->gridColor();

    if (ImGui::colorEdit3("Grid Color", gc))
      _editor->setGridColor(gc);
  }
  ImGui::End();
}

void
SceneWindowBase::preview(Camera& camera)
{
  if (!_showPreview)
    return;

  const auto hv = height();
  const auto wv = width();
  const auto ar = (float)wv / hv;

  camera.setAspectRatio(ar);

  const auto hp = (float)hv / 3;
  const auto wp = hp * ar;

  const auto& style = ImGui::GetStyle();
  auto wt = wp + (style.WindowPadding.x * 2);
  auto ht = hp + (style.WindowPadding.y + style.FramePadding.y) * 2 +
    ImGui::GetFontSize();

  ImGui::SetNextWindowPos({((float)wv - wt) / 2, (float)hv - (ht + 2)});
  ImGui::SetNextWindowSize({wt, ht});
  if (_fbo == nullptr)
    _fbo = new GLTextureFramebuffer{wv, hv};
  _fbo->use();
  {
    Reference<Camera> ec{_editor->camera()};

    _editor->setCamera(camera);
    _editor->render();
    drawAttachments();
    _editor->setCamera(*ec);
    // Update to continue drawing in the same frame
    _editor->update();
  }
  _fbo->disuse();
  ImGui::Begin("Preview", nullptr, ImGuiWindowFlags_NoResize);
  ImGui::Image((void*)(intptr_t)_fbo->texture(), {wp, hp}, {0, 1}, {1, 0});
  ImGui::End();
}

bool
SceneWindowBase::showErrorMessage(const char* message) const
{
  auto open = false;

  ImGui::OpenPopup("Error Message");
  if (ImGui::BeginPopupModal("Error Message",
    nullptr,
    ImGuiWindowFlags_AlwaysAutoResize))
  {
    open = true;
    ImGui::Text("%s", message);
    ImGui::Separator();
    if (ImGui::Button("Close"))
    {
      ImGui::CloseCurrentPopup();
      open = false;
    }
    ImGui::EndPopup();
  }
  return open;
}

bool
SceneWindowBase::windowResizeEvent(int width, int height)
{
  _editor->setImageSize(width, height);
  //if (_fbo && (_fbo->width() < width || _fbo->height() < height))
    _fbo = nullptr;
  return onResize(width, height);
}

constexpr auto CAMERA_RES = 0.01f;
constexpr auto ZOOM_SCALE = 1.01f;

bool
SceneWindowBase::scrollEvent(double dx, double dy)
{
  (void)dx;
  if (ImGui::GetIO().WantCaptureMouse)
    return false;
  _editor->zoom(dy < 0 ? 1.0f / ZOOM_SCALE : ZOOM_SCALE);
  return true;
}

bool
SceneWindowBase::mouseButtonInputEvent(int button, int actions, int mods)
{
  (void)mods;
  if (ImGui::GetIO().WantCaptureMouse)
    return false;

  auto active = actions == GLFW_PRESS;

  cursorPosition(_mouse.px, _mouse.py);
  if (button == GLFW_MOUSE_BUTTON_LEFT && !active)
    return onMouseLeftPress(_mouse.px, _mouse.py);
  if (button == GLFW_MOUSE_BUTTON_RIGHT)
    _dragFlags.enable(DragBits::Rotate, active);
  else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    _dragFlags.enable(DragBits::Pan, active);
  return true;
}

bool
SceneWindowBase::mouseMoveEvent(double xPos, double yPos)
{
  if (!uint32_t(_dragFlags))
    return false;
  _mouse.cx = (int)xPos;
  _mouse.cy = (int)yPos;

  const auto dx = (_mouse.cx - _mouse.px);
  const auto dy = (_mouse.cy - _mouse.py);

  _mouse.px = _mouse.cx;
  _mouse.py = _mouse.cy;
  if (dx != 0 || dy != 0)
  {
    if (_dragFlags.isSet(DragBits::Rotate))
    {
      const auto da = -_editor->camera()->viewAngle() * CAMERA_RES;
      isKeyPressed(GLFW_KEY_LEFT_ALT) ?
        _editor->orbit(dy * -da, dx * da) :
        _editor->rotateView(dy * -da, dx * da);
    }
    if (_dragFlags.isSet(DragBits::Pan))
    {
      const auto dt = _editor->orbitDistance() * CAMERA_RES;
      _editor->pan(dt * math::sign(dx), dt * math::sign(dy), 0);
    }
  }
  return true;
}

bool
SceneWindowBase::keyInputEvent(int key, int action, int mods)
{
  (void)mods;
  if (ImGui::GetIO().WantCaptureKeyboard || action == GLFW_RELEASE)
    return false;

  const auto delta = _editor->orbitDistance() * CAMERA_RES;
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
      return onKeyPress(key, mods);
  }
  _editor->pan(d);
  return true;
}

namespace
{ // begin namespace

inline auto
viewportToNDC(int x, int y)
{
  GLint v[4];

  glGetIntegerv(GL_VIEWPORT, v);

  const auto xn = float(x - v[0]) * 2.0f / float(v[2]) - 1.0f;
  const auto yn = float(y - v[1]) * 2.0f / float(v[3]) - 1.0f;

  return vec4f{xn, yn, -1, 1};
}

} // end namespace

Material*
SceneWindowBase::createMaterial()
{
  auto& map = Assets::materials();
  auto material = new Material{Color::white};

  material->setName("Material %d", int(map.size()));
  map.emplace(material->name(), material);
  return material;
}

Ray3f
SceneWindowBase::makeRay(int x, int y) const
{
  auto camera = _editor->camera();
  mat4f m{vpMatrix(camera)};

  m.invert();

  auto p = normalize(m * viewportToNDC(x, height() - y));
  Ray3f ray{camera->position(), camera->directionOfProjection()};

  if (camera->projectionType() == Camera::Perspective)
    ray.direction = (p - ray.origin).versor();
  else
    ray.origin = p - ray.direction * camera->nearPlane();
  return ray;
}

void
SceneWindowBase::inspectCamera(Camera& camera)
{
  static const char* projectionNames[]{"Perspective", "Orthographic"};
  auto cp = camera.projectionType();

  if (ImGui::BeginCombo("Projection", projectionNames[cp]))
  {
    for (auto i = 0; i < IM_ARRAYSIZE(projectionNames); ++i)
    {
      auto selected = cp == i;

      if (ImGui::Selectable(projectionNames[i], selected))
        cp = (Camera::ProjectionType)i;
      if (selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
  camera.setProjectionType(cp);
  if (cp == Camera::Perspective)
  {
    auto fov = camera.viewAngle();

    if (ImGui::SliderFloat("View Angle",
      &fov,
      Camera::minAngle,
      Camera::maxAngle,
      "%.0f deg"))
      camera.setViewAngle(fov);
  }
  else
  {
    auto h = camera.height();

    if (ImGui::DragFloat("Height",
      &h,
      Camera::minHeight * 10.0f,
      Camera::minHeight,
      math::Limits<float>::inf()))
      camera.setHeight(h);
  }

  float n;
  float f;

  camera.clippingPlanes(n, f);

  if (ImGui::DragFloatRange2("Clipping Planes",
    &n,
    &f,
    Camera::minDepth,
    Camera::minDepth,
    math::Limits<float>::inf(),
    "Near: %.2f",
    "Far: %.2f"))
  {
    if (n <= 0)
      n = Camera::minDepth;
    if (f - n < Camera::minDepth)
      f = n + Camera::minDepth;
    camera.setClippingPlanes(n, f);
  }
}

void
SceneWindowBase::inspectLight(Light& light)
{
  {
    bool state = light.isTurnedOn();

    ImGui::Checkbox("Turned On", &state);
    light.turnOn(state);
  }

  static const char* lightTypes[]{"Directional", "Point", "Spot"};
  auto lt = light.type();

  if (ImGui::BeginCombo("Type", lightTypes[(int)lt]))
  {
    for (auto i = 0; i < IM_ARRAYSIZE(lightTypes); ++i)
    {
      bool selected = (int)lt == i;

      if (ImGui::Selectable(lightTypes[i], selected))
        lt = (Light::Type)i;
      if (selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
  light.setType(lt);
  ImGui::colorEdit3("Color", light.color);
  if (lt == Light::Type::Directional)
    return;
  {
    auto range = light.range();

    ImGui::DragFloat("Range", &range, 0.1f, 0, 1000);

    bool state = range == 0;

    ImGui::SameLine();
    ImGui::Checkbox("Infinite", &state);
    if (state)
      range = 0;
    light.setRange(range);
    light.flags.enable(Light::Infinite, state);
  }

  static const char* falloff[]{"Constant", "Linear", "Quadratic"};
  auto f = light.falloff;

  if (ImGui::BeginCombo("Falloff", falloff[(int)f]))
  {
    for (auto i = 0; i < IM_ARRAYSIZE(falloff); ++i)
    {
      bool selected = (int)f == i;

      if (ImGui::Selectable(falloff[i], selected))
        f = (Light::Falloff)i;
      if (selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
  light.falloff = f;
  if (lt == Light::Type::Point)
    return;

  auto angle = light.spotAngle();

  if (ImGui::SliderFloat("Spot Angle",
    &angle,
    Light::minSpotAngle,
    Light::maxSpotAngle,
    "%.0f deg"))
    light.setSpotAngle(angle);
}

void
SceneWindowBase::inspectMaterial(Material& material)
{
  ImGui::colorEdit3("Ambient", material.ambient);
  ImGui::colorEdit3("Diffuse", material.diffuse);
  ImGui::colorEdit3("Spot", material.spot);
  ImGui::DragFloat("Shine", &material.shine, 1, 1, 1000);
  ImGui::colorEdit3("Specular", material.specular);
  ImGui::colorEdit3("Transparency", material.transparency);
  ImGui::DragFloat("IOR", &material.ior, 0.01f, 1, 5);
}

} // end namespace cg
