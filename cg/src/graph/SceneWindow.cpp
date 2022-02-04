//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2020, 2022 Orthrus Group.                         |
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
// Class definition for scene window base.
//
// Author: Paulo Pagliosa
// Last revision: 04/02/2022

#include "graph/SceneWindow.h"
#include "graphics/Assets.h"
#include <cassert>

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

inline void
inputText(const char* label, const char* text)
{
  constexpr auto bufferSize = 64;
  char buffer[bufferSize];

  snprintf(buffer, bufferSize, "%s", text);
  InputText(label, buffer, bufferSize, ImGuiInputTextFlags_ReadOnly);
}

} // end namespace ImGui

namespace cg
{ // begin namespace cg

namespace graph
{ // begin namespace graph


/////////////////////////////////////////////////////////////////////
//
// SceneWindow implementation
// ===========
void
SceneWindow::beginInitialize()
{
  // do nothing
}

void
SceneWindow::endInitialize()
{
  // do nothing
}

void
SceneWindow::initializeScene()
{
  // do nothing
}

void
SceneWindow::initialize()
{
  beginInitialize();

  auto scene = Scene::New();

  SceneObjectBuilder::setScene(*scene);
  _editor = new SceneEditor{*scene};
  _currentNode = scene;

  auto w = width(), h = height();

  _editor->setImageSize(w, h);
  _editor->setDefaultView((float)w / (float)h);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0f, 1.0f);
  glEnable(GL_LINE_SMOOTH);
  endInitialize();
  initializeScene();
}

void
SceneWindow::setScene(Scene& scene)
{
  if (&scene != _scene)
  {
    SceneObjectBuilder::setScene(scene);
    _editor->setScene(scene);
    _currentNode = &scene;
  }
}

Scene*
SceneWindow::createScene()
{
  auto scene = Scene::New();

  setScene(*scene);
  initializeScene();
  return _scene;
}

void
SceneWindow::renderScene()
{
  // do nothing
}

void
SceneWindow::preview(Camera& camera)
{
  if (!_showPreview)
    return;

  const auto hv = height();
  const auto wv = width();
  const auto ar = (float)wv / hv;
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
    _editor->setCamera(*ec);
  }
  _fbo->disuse();
  ImGui::Begin("Preview", nullptr, ImGuiWindowFlags_NoResize);
  ImGui::Image((void*)(intptr_t)_fbo->texture(), {wp, hp}, {0, 1}, {1, 0});
  ImGui::End();
}

void
SceneWindow::drawComponents(const SceneObject& object)
{
  const auto& components = object.components();

  // Iterate the object components skipping its transform
  for (auto end = components.end(), cit = ++components.begin(); cit != end;)
  {
    Component* c{*cit++};

    if (auto proxy = graph::asLight(c))
      _editor->drawLight(*proxy);
    else if (auto proxy = graph::asCamera(c))
    {
      _editor->drawCamera(*proxy);
      preview(*proxy->camera());
    }
    else if (auto proxy = graph::asPrimitive(c))
    {
      auto p = proxy->mapper()->primitive();

      assert(p != nullptr);
      if (auto mesh = p->tesselate())
      {
        _editor->setPolygonMode(GLGraphics3::LINE);
        _editor->setMeshColor(_selectedWireframeColor);
        _editor->drawMesh(*mesh, p->localToWorldMatrix(), p->normalMatrix());
      }
    }
  }
}

void
SceneWindow::drawSelectedObject(const SceneObject& object)
{
  if (!object.visible())
    return;
  drawComponents(object);
  for (auto& child : object.children())
    drawSelectedObject(*child);
}

void
SceneWindow::render()
{
  if (_viewMode != ViewMode::Editor)
  {
    renderScene();
    return;
  }
  _editor->render();
  if (_editor->showGround)
    _editor->drawXZPlane(10, 1);
  if (auto object = _currentNode->as<SceneObject>())
  {
    drawSelectedObject(*object);
    _editor->drawTransform(*object->transform());
  }
}

void
SceneWindow::showErrorMessage(const char* message) const
{
  ImGui::OpenPopup("Error Message");
  if (ImGui::BeginPopupModal("Error Message",
    nullptr,
    ImGuiWindowFlags_AlwaysAutoResize))
  {
    ImGui::Text(message);
    ImGui::Separator();
    if (ImGui::Button("Close"))
      ImGui::CloseCurrentPopup();
    ImGui::EndPopup();
  }
}

void
SceneWindow::createObjectMenu()
{
  // do nothing
}

inline void
SceneWindow::createObjectButton()
{
  if (ImGui::Button("Create###CreateObjectButton"))
    ImGui::OpenPopup("CreateObjectPopup");
  if (ImGui::BeginPopup("CreateObjectPopup"))
  {
    if (ImGui::MenuItem("Empty Object"))
      createEmptyObject();
    ImGui::Separator();
    createObjectMenu();
    if (ImGui::BeginMenu("Light"))
    {
      if (ImGui::MenuItem("Directional Light"))
        createLightObject(Light::Type::Directional);
      if (ImGui::MenuItem("Point Light"))
        createLightObject(Light::Type::Point);
      if (ImGui::MenuItem("Spotlight"))
        createLightObject(Light::Type::Spot);
      ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Camera"))
      createCameraObject();
    ImGui::EndPopup();
  }
}

inline bool
SceneWindow::treeNode(SceneNode& node, ImGuiTreeNodeFlags flags)
{
  if (&node == _currentNode)
    flags |= ImGuiTreeNodeFlags_Selected;

  auto open = ImGui::TreeNodeEx(&node, flags, node.name());

  if (ImGui::IsItemClicked())
    _currentNode = &node;
  return open;
}

inline bool
SceneWindow::deleteObjectPopup(SceneObject& object)
{
  auto deleted = false;

  if (ImGui::BeginPopupContextItem())
  {
    if (ImGui::MenuItem("Delete###DeleteObject"))
    {
      auto parent = object.parent();

      if (parent->parent() == nullptr)
        _currentNode = parent->scene();
      else
        _currentNode = parent;
      parent->removeChild(&object);
      deleted = true;
    }
    ImGui::EndPopup();
  }
  return deleted;
}

namespace
{ // begin namespace

bool
dropSceneObject(SceneObject& target)
{
  if (ImGui::BeginDragDropTarget() && target.movable())
  {
    if (auto* payload = ImGui::AcceptDragDropPayload("SceneObject"))
    {
      auto object = *(SceneObject**)payload->Data;

      if (&target != object)
      {
        object->setParent(&target);
        return true;
      }
    }
    ImGui::EndDragDropTarget();
  }
  return false;
}

} // end namespace

bool
SceneWindow::objectHierarchy(SceneObject& object)
{
  for (auto& child : object.children())
  {
    ImGuiTreeNodeFlags flags{ImGuiTreeNodeFlags_OpenOnArrow};
    auto hasChildren = child.childrenCount() > 0;

    if (!hasChildren)
      flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

    auto open = treeNode(child, flags);
    auto movable = child.movable() && !deleteObjectPopup(child);

    if (movable && ImGui::BeginDragDropSource())
    {
      auto* data = &child;

      ImGui::Text(child.name());
      ImGui::SetDragDropPayload("SceneObject", &data, sizeof(SceneObject*));
      ImGui::EndDragDropSource();
    }

    auto dropped = dropSceneObject(child);

    if (open)
    {
      if (movable)
        dropped |= objectHierarchy(child);
      if (hasChildren)
        ImGui::TreePop();
    }
    if (dropped)
      return true;
  }
  return false;
}

void
SceneWindow::hierarchyWindow(const char* title)
{
  assert(title != nullptr);
  ImGui::Begin(title);
  createObjectButton();
  ImGui::Separator();
  if (treeNode(*_scene, ImGuiTreeNodeFlags_OpenOnArrow))
  {
    auto root = _scene->root();

    dropSceneObject(*root);
    objectHierarchy(*root);
    ImGui::TreePop();
  }
  ImGui::End();
}

void
SceneWindow::inspectTransform(Transform& transform)
{
  vec3f temp;

  temp = transform.localPosition();
  if (ImGui::dragVec3("Position", temp))
    transform.setLocalPosition(temp);
  temp = transform.localEulerAngles();
  if (ImGui::dragVec3("Rotation", temp))
    transform.setLocalEulerAngles(temp);
  temp = transform.localScale();
  if (ImGui::dragVec3("Scale", temp, 0.001f, math::Limits<float>::inf()))
    transform.setLocalScale(temp);
}

void
SceneWindow::inspectComponent(Component& component)
{
  auto typeName = component.typeName();
  auto notDelete{true};
  auto open = component.erasable() ?
    ImGui::CollapsingHeader(typeName, &notDelete) :
    ImGui::CollapsingHeader(typeName);

  if (!notDelete)
    component.sceneObject()->removeComponent(typeName);
  else if (open)
  {
    if (typeName == "Transform")
      inspectTransform((Transform&)component);
    else if (auto function = inspectFunction(component))
      function(component);
  }
}

void
SceneWindow::inspectCamera(Camera& camera)
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
      "%.0f deg",
      1.0f))
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
SceneWindow::inspectCamera(CameraProxy& proxy)
{
  auto camera = proxy.camera();

  {
    auto isCurrent = camera == CameraProxy::current();

    ImGui::Checkbox("Current", &isCurrent);
    CameraProxy::setCurrent(isCurrent ? camera : nullptr);
  }
  inspectCamera(*camera);
}

void
SceneWindow::inspectLight(Light& light)
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
    "%.0f deg",
    1.0f))
    light.setSpotAngle(angle);
}

void
SceneWindow::inspectLight(LightProxy& proxy)
{
  inspectLight(*proxy.light());
}

void
SceneWindow::inspectMaterial(Material& material)
{
  ImGui::colorEdit3("Ambient", material.ambient);
  ImGui::colorEdit3("Diffuse", material.diffuse);
  ImGui::colorEdit3("Spot", material.spot);
  ImGui::DragFloat("Shine", &material.shine, 1, 0, 1000);
  ImGui::colorEdit3("Specular", material.specular);
}

void
SceneWindow::inspectPrimitive(TriangleMeshProxy& proxy)
{
  ImGui::inputText("Mesh", proxy.meshName());
  if (ImGui::BeginDragDropTarget())
  {
    if (auto* payload = ImGui::AcceptDragDropPayload("TriangleMesh"))
    {
      auto mit = *(MeshMapIterator*)payload->Data;

      assert(mit->second != nullptr);
      proxy.setMesh(*mit->second, mit->first);
    }
    ImGui::EndDragDropTarget();
  }
  ImGui::SameLine();
  if (ImGui::Button("...###PrimitiveMesh"))
    ImGui::OpenPopup("PrimitiveMeshPopup");
  if (ImGui::BeginPopup("PrimitiveMeshPopup"))
  {
    if (auto& meshes = Assets::meshes(); !meshes.empty())
    {
      for (auto mit = meshes.begin(); mit != meshes.end(); ++mit)
        if (ImGui::Selectable(mit->first.c_str()))
          proxy.setMesh(*Assets::loadMesh(mit), mit->first);
    }
    ImGui::EndPopup();
  }
  ImGui::Separator();

  auto primitive = proxy.mapper()->primitive();
  auto material = primitive->material();

  ImGui::inputText("Material", material->name());
  if (ImGui::BeginDragDropTarget())
  {
    if (auto* payload = ImGui::AcceptDragDropPayload("Material"))
    {
      auto mit = *(MaterialMapIterator*)payload->Data;

      assert(mit->second != nullptr);
      primitive->setMaterial(material = mit->second);
    }
    ImGui::EndDragDropTarget();
  }
  inspectMaterial(*material);
}

void
SceneWindow::editorView()
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
    static int sm;

    ImGui::Combo("Shading Mode", &sm, "None\0Flat\0Gouraud\0\0");
    // TODO

    static Color edgeColor;
    static bool showEdges;

    ImGui::colorEdit3("Edges", edgeColor);
    ImGui::SameLine();
    ImGui::Checkbox("###showEdges", &showEdges);
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

Component*
SceneWindow::addComponentMenu()
{
  return nullptr;
}

inline void
SceneWindow::addComponentButton(SceneObject& object)
{
  auto ok = true;

  if (ImGui::Button("Add Component"))
    ImGui::OpenPopup("AddComponentPopup");
  if (ImGui::BeginPopup("AddComponentPopup"))
  {
    auto component = addComponentMenu();

    if (ImGui::MenuItem("Light"))
      component = LightProxy::New();
    if (ImGui::MenuItem("Camera"))
      component = CameraProxy::New();
    if (nullptr != component)
      ok = object.addComponent(component);
    ImGui::EndPopup();
  }
  if (!ok)
    showErrorMessage("Unable to add this component type.");
}

inline void
SceneWindow::inspectSceneObject(SceneObject& object)
{
  addComponentButton(object);
  ImGui::Separator();
  ImGui::objectNameInput(object);
  ImGui::SameLine();

  bool state{object.visible()};

  ImGui::Checkbox("###visible", &state);
  object.setVisible(state);
  ImGui::Separator();

  auto& components = object.components();

  // Attention: do *NOT* modify this for (the iterator postfix increment
  // is necessary to avoid an exception in case the component is removed
  // by the user during the object inspection)
  for (auto end = components.end(), cit = components.begin(); cit != end;)
    inspectComponent(*((cit++)->get()));
}

inline void
SceneWindow::inspectScene()
{
  ImGui::objectNameInput(*_scene);
  ImGui::Separator();
  if (ImGui::CollapsingHeader("Environment"))
  {
    ImGui::colorEdit3("Background", _scene->backgroundColor);
    ImGui::colorEdit3("Ambient Light", _scene->ambientLight);
  }
}

inline void
SceneWindow::inspectCurrentNode()
{
  if (_currentNode == nullptr)
    return;
  if (auto sceneObject = _currentNode->as<SceneObject>())
    inspectSceneObject(*sceneObject);
  else if (_currentNode->as<Scene>())
    inspectScene();
}

void
SceneWindow::inspectorWindow(const char* title)
{
  assert(title != nullptr);
  ImGui::Begin(title);
  inspectCurrentNode();
  ImGui::End();
}

void
SceneWindow::assetsWindow()
{
  if (!_showAssets)
    return;
  ImGui::Begin("Assets");
  if (ImGui::CollapsingHeader("Meshes"))
  {
    auto& meshes = Assets::meshes();

    for (auto mit = meshes.begin(); mit != meshes.end(); ++mit)
    {
      auto name = mit->first.c_str();
      auto selected = false;

      ImGui::Selectable(name, &selected);
      if (ImGui::BeginDragDropSource())
      {
        Assets::loadMesh(mit);
        ImGui::Text(name);
        ImGui::SetDragDropPayload("TriangleMesh", &mit, sizeof(mit));
        ImGui::EndDragDropSource();
      }
    }
  }
  ImGui::Separator();
  if (ImGui::CollapsingHeader("Materials"))
  {
    auto& materials = Assets::materials();

    for (auto mit = materials.begin(); mit != materials.end(); ++mit)
    {
      auto name = mit->first.c_str();
      auto selected = false;

      ImGui::Selectable(name, &selected);
      if (ImGui::BeginDragDropSource())
      {
        ImGui::Text(name);
        ImGui::SetDragDropPayload("Material", &mit, sizeof(mit));
        ImGui::EndDragDropSource();
      }
    }
  }
  ImGui::End();
}

bool
SceneWindow::onResize(int width, int height)
{
  return true;
}

bool
SceneWindow::windowResizeEvent(int width, int height)
{
  _editor->setImageSize(width, height);
  if (_fbo && (_fbo->width() < width || _fbo->height() < height))
    _fbo = nullptr;
  return onResize(width, height);
}

constexpr auto CAMERA_RES = 0.01f;
constexpr auto ZOOM_SCALE = 1.01f;

bool
SceneWindow::scrollEvent(double dx, double dy)
{
  if (ImGui::GetIO().WantCaptureMouse)
    return false;
  _editor->zoom(dy < 0 ? 1.0f / ZOOM_SCALE : ZOOM_SCALE);
  return true;
}

SceneObject*
SceneWindow::pickObject(SceneObject* object, const Ray3f& ray, float& t) const
{
  if (!object->visible())
    return nullptr;

  SceneObject* nearest{};

  for (auto& component : object->components())
    if (auto proxy = dynamic_cast<PrimitiveProxy*>(&*component))
    {
      auto p = proxy->mapper()->primitive();

      if (p->bounds().size().min() == 0)
        puts("Unable to pick scene object");
      else
      {
        Intersection hit;

        if (p->intersect(ray, hit) && hit.distance < t)
        {
          t = hit.distance;
          nearest = object;
        }
      }
      break;
    }
  for (auto& child : object->children())
    if (auto temp = pickObject(&child, ray, t))
      nearest = temp;
  return nearest;
}

SceneObject*
SceneWindow::pickObject(int x, int y) const
{
  auto ray = makeRay(x, y);
  auto t = math::Limits<float>::inf();

  return pickObject(_scene->root(), ray, t);
}

bool
SceneWindow::mouseButtonInputEvent(int button, int actions, int mods)
{
  if (ImGui::GetIO().WantCaptureMouse)
    return false;
  (void)mods;

  auto active = actions == GLFW_PRESS;

  cursorPosition(_mouse.px, _mouse.py);
  if (button == GLFW_MOUSE_BUTTON_LEFT && !active)
  {
    if (auto o = pickObject(_mouse.px, _mouse.py))
      if (o->selectable())
      {
        if (auto p = _currentNode->as<SceneObject>())
          p->setSelected(false);
        o->setSelected(true);
        _currentNode = o;
      }
    return true;
  }
  if (_viewMode != ViewMode::Editor)
    return false;
  if (button == GLFW_MOUSE_BUTTON_RIGHT)
    _dragFlags.enable(DragBits::Rotate, active);
  else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    _dragFlags.enable(DragBits::Pan, active);
  return true;
}

bool
SceneWindow::mouseMoveEvent(double xPos, double yPos)
{
  if (!_dragFlags)
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
SceneWindow::keyInputEvent(int key, int action, int mods)
{
  if (ImGui::GetIO().WantCaptureKeyboard ||
    action == GLFW_RELEASE ||
    _viewMode != ViewMode::Editor)
    return false;
  (void)mods;

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
      return false;
  }
  _editor->pan(d);
  return true;
}

namespace
{ // begin namespace

inline auto
normalize(const vec4f& p)
{
  return vec3f{p} * math::inverse(p.w);
}

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

Ray3f
SceneWindow::makeRay(int x, int y) const
{
  auto c = _editor->camera();
  mat4f m{vpMatrix(c)};

  m.invert();

  auto p = normalize(m * viewportToNDC(x, height() - y));
  Ray r{c->position(), c->directionOfProjection()};

  if (c->projectionType() == Camera::Perspective)
    r.direction = (p - r.origin).versor();
  else
    r.origin = p - r.direction * c->nearPlane();
  return r;
}

} // end namespace graph

} // end namespace cg
