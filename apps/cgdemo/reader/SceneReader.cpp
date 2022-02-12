//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2007, 2022 Paulo Pagliosa.                        |
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
// OVERVIEW: SceneReader.cpp
// ========
// Source file for scene reader.
//
// Author: Paulo Pagliosa
// Last revision: 04/02/2022

#include "SceneReader.h"

namespace cg::parser
{ // begin namespace cg::parser


/////////////////////////////////////////////////////////////////////
//
// SceneReader implementation
// ===========
void
SceneReader::execute()
{
  Assets::initialize();
  Reader::execute();
}

Reader::Parser*
SceneReader::makeParser()
{
  return new Parser{*this};
}


/////////////////////////////////////////////////////////////////////
//
// SceneReader::Parser implementation
// ===================
DEFINE_KEYWORD_TABLE(SceneReader::Parser, Reader::Parser)
  KEYWORD("ambient", _AMBIENT, 0)
  KEYWORD("angle", _ANGLE, 0)
  KEYWORD("aspect", _ASPECT, 0)
  KEYWORD("background", _BACKGROUND, 0)
  KEYWORD("box", _COMPONENT, _BOX)
  KEYWORD("camera", _COMPONENT, _CAMERA)
  KEYWORD("color", _COLOR, 0)
  KEYWORD("depth", _DEPTH, 0)
  KEYWORD("diffuse", _DIFFUSE, 0)
  KEYWORD("directional", _DIRECTIONAL, 0)
  KEYWORD("environment", _ENVIRONMENT, 0)
  KEYWORD("falloff", _FALLOFF, 0)
  KEYWORD("finish", _FINISH, 0)
  KEYWORD("height", _HEIGHT, 0)
  KEYWORD("light", _COMPONENT, _LIGHT)
  KEYWORD("material", _MATERIAL, 0)
  KEYWORD("mesh", _COMPONENT, _MESH)
  KEYWORD("object", _OBJECT, 0)
  KEYWORD("parallel", _PARALLEL, 0)
  KEYWORD("perspective", _PERSPECTIVE, 0)
  KEYWORD("point", _POINT, 0)
  KEYWORD("position", _POSITION, 0)
  KEYWORD("range", _RANGE, 0)
  KEYWORD("rotation", _ROTATION, 0)
  KEYWORD("scale", _SCALE, 0)
  KEYWORD("scene", _SCENE, 0)
  KEYWORD("sphere", _COMPONENT, _SPHERE)
  KEYWORD("shine", _SHINE, 0)
  KEYWORD("specular", _SPECULAR, 0)
  KEYWORD("spot", _SPOT, 0)
  KEYWORD("transform", _TRANSFORM, 0)
  END_KEYWORD_TABLE;

DEFINE_ERROR_MESSAGE_TABLE(SceneReader::Parser, Reader::Parser)
  ERROR_MESSAGE(MATERIAL_ALREADY_DEFINED,
    "Material '%s' already defined")
  ERROR_MESSAGE(COLOR_EXPECTED,
    "'color' expected")
  ERROR_MESSAGE(COMPONENT_ALREADY_DEFINED,
    "Component '%s' already defined")
  ERROR_MESSAGE(INVALID_VALUE_FOR,
    "Invalid value for '%s'")
  ERROR_MESSAGE(COULD_NOT_FIND_MESH,
    "Could not find mesh '%s'")
  ERROR_MESSAGE(COULD_NOT_FIND_MATERIAL,
    "Could not find material '%s'")
END_ERROR_MESSAGE_TABLE;

inline void
SceneReader::Parser::preamble()
{
  for (;;)
    switch (_token)
    {
      case _INCLUDE:
        include();
        break;
      case _DEFINE:
        declaration();
        break;
      case _MATERIAL:
        parseMaterial();
        break;
      default:
        return;
    }
}

void
SceneReader::Parser::start()
{
  try
  {
    preamble();
    if (_token == _SCENE)
      if (_scene != nullptr)
        error(MULTIPLE_SCENE_DEFINITION);
      else
        parseScene();
    if (_token != _EOF)
      if (_token < 256)
        error(UNEXPECTED_CHAR, _token);
      else
        error(SYNTAX);
  }
  catch (const std::exception& e)
  {
    _reader->_scene = nullptr;
    throw e;
  }
}

void
SceneReader::Parser::declaration()
{
  // _DEFINE
  advance();

  auto name = matchName();

  if (_token == '=')
    advance();
  define(name, initializer());
}

inline void
SceneReader::Parser::parseSurface(Material& material, const Color& color)
{
  // _FINISH
  advance();
  match('{');
  for (;;)
    switch (_token)
    {
      case _AMBIENT:
        advance();
        if (auto ka = matchFloat(); ka < 0 || ka > 1)
          error(INVALID_VALUE_FOR, "ambient");
        else
          material.ambient = color * ka;
        break;
      case _DIFFUSE:
        advance();
        if (auto kd = matchFloat(); kd < 0 || kd > 1)
          error(INVALID_VALUE_FOR, "diffuse");
        else
          material.diffuse = color * kd;
        break;
      case _SPOT:
        advance();
        if (auto ks = matchFloat(); ks < 0 || ks > 1)
          error(INVALID_VALUE_FOR, "spot");
        else
          material.spot = color * ks;
        break;
      case _SHINE:
        advance();
        if (auto ns = matchFloat(); ns < 0)
          error(INVALID_VALUE_FOR, "shine");
        else
          material.shine = ns;
        break;
      case _SPECULAR:
        advance();
        if (auto ks = matchFloat(); ks < 0 || ks > 1)
          error(INVALID_VALUE_FOR, "specular");
        else
          material.specular = color * ks;
        break;
      default:
        matchEndOfBlock();
        return;
    }
}

void
SceneReader::Parser::parseMaterial()
{
  // _MATERIAL
  advance();

  auto name = matchString();

  if (_reader->findMaterial(name) != nullptr)
    error(MATERIAL_ALREADY_DEFINED, name.c_str());
  match('{');
  if (_token != _COLOR)
    error(COLOR_EXPECTED);
  advance();

  auto color = matchColor();
  Reference<Material> material = new Material{color};

  if (_token == _FINISH)
    parseSurface(*material, color);
  matchEndOfBlock();
  material->setName(name.c_str());
  _reader->materials.emplace(name, material);
}

inline void
SceneReader::Parser::parseSceneEnvironment()
{
  // _ENVIRONMENT
  advance();
  match('{');
  for (;;)
    if (_token == _AMBIENT)
    {
      advance();
      _scene->ambientLight = matchColor();
    }
    else if (_token == _BACKGROUND)
    {
      advance();
      _scene->backgroundColor = matchColor();
    }
    else
      break;
  matchEndOfBlock();
}

void
SceneReader::Parser::parseScene()
{
  // _SCENE
  advance();

  auto sceneName = matchOptionalString();
  auto scene = graph::Scene::New(sceneName.c_str());

  setScene(*scene);
  match('{');
  beginBlock();
  while (_token == _DEFINE)
    declaration();
  if (_token == _ENVIRONMENT)
    parseSceneEnvironment();
  parseObjectBlock(*scene->root());
  _reader->_scene = scene;
}

void
SceneReader::Parser::parseObjectBlock(graph::SceneObject& object)
{
  for (;;)
    if (_token == _OBJECT)
      parseObject(object);
    else if (_token == _DEFINE)
      declaration();
    else
      break;
  matchEndOfBlock();
  endBlock();
}

inline void
SceneReader::Parser::parseTransform(graph::SceneObject& object)
{
  // _TRANSFORM
  advance();
  match('{');
  for (auto t = object.transform();;)
    switch (_token)
    {
      case _POSITION:
        advance();
        t->setLocalPosition(matchVec3());
        break;
      case _ROTATION:
        advance();
        t->setLocalEulerAngles(matchVec3());
        break;
      case _SCALE:
        advance();
        if (auto s = matchVec3(); s.x > 0 && s.y > 0 && s.z > 0)
          t->setLocalScale(s);
        else
          error(INVALID_VALUE_FOR, "scale");
        break;
      default:
        matchEndOfBlock();
        return;
    }
}

inline void
SceneReader::Parser::parseChildObjectBlock(graph::SceneObject& object)
{
  while (_token == _DEFINE)
    declaration();
  if (_token == _TRANSFORM)
    parseTransform(object);
  while (_token == _COMPONENT)
    parseComponent(object);
  parseObjectBlock(object);
}

void
SceneReader::Parser::parseObject(graph::SceneObject& object)
{
  // _OBJECT
  advance();

  auto childName = matchOptionalString();
  auto child = createEmptyObject();

  if (!childName.empty())
    child->setName(childName.c_str());
  child->setParent(&object);
  if (_token == _COMPONENT)
    parseComponent(*child);
  else
  {
    match('{');
    beginBlock();
    parseChildObjectBlock(*child);
  }
}

inline Reference<graph::CameraProxy>
SceneReader::Parser::matchCamera()
{
  // _COMPONENT (_CAMERA)
  advance();
  match('{');

  Reference<graph::CameraProxy> proxy{graph::CameraProxy::New()};

  for (auto camera = proxy->camera();;)
  {
    if (_token == _PERSPECTIVE)
      advance();
    else if (_token == _PARALLEL)
    {
      advance();
      camera->setProjectionType(Camera::Parallel);
    }
    switch (_token)
    {
      case _ANGLE:
        advance();
        if (auto angle = matchFloat(); angle > 0)
          camera->setViewAngle(angle);
        else
          error(INVALID_VALUE_FOR, "angle");
        break;
      case _ASPECT:
        advance();
        if (auto aspect = matchFloat(); aspect > 0)
          camera->setAspectRatio(aspect);
        else
          error(INVALID_VALUE_FOR, "aspect");
        break;
      case _DEPTH:
        advance();
        if (auto z = matchVec2(); z.x > 0 && z.y > z.x)
          camera->setClippingPlanes(z.x, z.y);
        else
          error(INVALID_VALUE_FOR, "depth");
        break;
      case _HEIGHT:
        advance();
        if (auto height = matchFloat(); height > 0)
          camera->setHeight(height);
        else
          error(INVALID_VALUE_FOR, "height");
        break;
      default:
        matchEndOfBlock();
        graph::CameraProxy::setCurrent(camera);
        return proxy;
    }
  }
}

inline Reference<graph::LightProxy>
SceneReader::Parser::matchLight()
{
  // _COMPONENT (_LIGHT)
  advance();
  match('{');

  Reference<graph::LightProxy> proxy{graph::LightProxy::New()};

  for (auto light = proxy->light();;)
  {
    if (_token == _DIRECTIONAL)
      advance();
    else if (_token == _POINT)
    {
      advance();
      light->setType(Light::Type::Point);
    }
    else if (_token == _SPOT)
    {
      advance();
      light->setType(Light::Type::Spot);
    }
    switch (_token)
    {
      case _COLOR:
        advance();
        light->color = matchColor();
        break;
      case _RANGE:
        advance();
        if (auto range = matchFloat(); range > 0)
          light->setRange(range);
        else
          error(INVALID_VALUE_FOR, "range");
        break;
      case _ANGLE:
        advance();
        if (auto angle = matchFloat(); angle > 0)
          light->setSpotAngle(angle);
        else
          error(INVALID_VALUE_FOR, "angle");
        break;
      case _FALLOFF:
        advance();
        light->falloff = (Light::Falloff)matchIndex(0, 2);
        break;
      default:
        matchEndOfBlock();
        return proxy;
    }
  }
}

inline Reference<graph::PrimitiveProxy>
SceneReader::Parser::matchPrimitive(int type)
{
  // _COMPONENT (_BOX, _SPHERE, _MESH)
  advance();

  String name;

  if (type == _BOX)
    name = "Box";
  else if (type == _SPHERE)
    name = "Sphere";
  else
    name = matchFilename();

  Reference<graph::PrimitiveProxy> proxy;

  if (auto mesh = Assets::loadMesh(name); nullptr == mesh)
    error(COULD_NOT_FIND_MESH, name.c_str());
  else
    proxy = makePrimitive(*mesh, name);
  if (_token == _MATERIAL)
  {
    advance();
    name = matchString();
    if (auto material = _reader->findMaterial(name); material == nullptr)
      error(COULD_NOT_FIND_MATERIAL, name.c_str());
    else
      proxy->mapper()->primitive()->setMaterial(material);
  }
  return proxy;
}

void
SceneReader::Parser::parseComponent(graph::SceneObject& object)
{
  Reference<graph::Component> component;
  auto type = (int)(size_t)_tokenValue.object;

  if (type == _CAMERA)
    component = matchCamera();
  else if (type == _LIGHT)
    component = matchLight();
  else
    component = matchPrimitive(type);

  auto typeName = component->typeName();

  if (object.addComponent<graph::Component>(component) == nullptr)
    error(COMPONENT_ALREADY_DEFINED, typeName);
}

} // end namespace cg::parser
