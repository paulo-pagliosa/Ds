//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2023 Paulo Pagliosa.                              |
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
// OVERVIEW: SceneWriter.cpp
// ========
// Source file for scene writer.
//
// Author: Paulo Pagliosa
// Last revision: 21/07/2023

#include "graph/CameraProxy.h"
#include "graph/LightProxy.h"
#include "graph/PrimitiveProxy.h"
#include "graphics/Assets.h"
#include "SceneWriter.h"

namespace cg::util
{ // begin namespace cg::util


/////////////////////////////////////////////////////////////////////
//
// SceneWriter implementation
// ===========
void
SceneWriter::writeName(const char* label, const char* name)
{
  beginLine();
  write(label);
  if (name && *name)
    write(" \"%s\"", name);
  endLine();
}

inline void
SceneWriter::writeEnvironment(const Scene& scene)
{
  beginBlock("environment");
  writeColor("ambient", scene.ambientLight);
  writeColor("background", scene.backgroundColor);
  endBlock();
}

namespace
{ // begin namespace

template <typename V>
inline bool
writeIfNotNull(SceneWriter& w, const char* label, const V& v)
{
  return v.x != 0 || v.y != 0 || v.z != 0 ?
    w.writeVec3(label, v), true : false;
}

template <typename V>
inline bool
writeScale(SceneWriter& w, const V& s)
{
  return s.x != 1 || s.y != 1 || s.z != 1 ?
    w.writeVec3("scale", s), true : false;
}

void
writeCamera(SceneWriter& w, const CameraProxy& proxy)
{
  w.beginBlock("camera");

  auto camera = proxy.camera();

  if (camera->projectionType() == Camera::Perspective)
  {
    w.writeLine("perspective");
    w.writeFloat("angle", camera->viewAngle());
  }
  else
  {
    w.writeLine("parallel");
    w.writeFloat("height", camera->height());
  }
  w.writeFloat("aspect", camera->aspectRatio());

  float F, B;

  camera->clippingPlanes(F, B);
  w.writeLine("depth vec2(%g,%g)", F, B);
  w.endBlock();
}

void
writeLight(SceneWriter& w, const LightProxy& proxy)
{
  w.beginBlock("light");

  auto light = proxy.light();
  auto lt = light->type();

  if (lt == Light::Type::Directional)
    w.writeLine("directional");
  else
  {
    if (lt == Light::Type::Spot)
    {
      w.writeLine("spot");
      w.writeFloat("angle", light->spotAngle());
    }
    else
      w.writeLine("point");
    w.writeFloat("range", light->range());
    w.writeLine("falloff %d", light->falloff);
  }
  w.writeColor("color", light->color);
  w.endBlock();
}

void
writePrimitive(SceneWriter& w, const TriangleMeshProxy& proxy)
{
  w.writeLine("mesh \"%s\" material \"%s\"",
    proxy.meshName(),
    proxy.mapper()->primitive()->material()->name());
}

} // end namespace

inline void
SceneWriter::writeTransform(const Transform& t)
{
  auto w = false;

  beginBlock("transform");
  w |= writeIfNotNull(*this, "position", t.position());
  w |= writeIfNotNull(*this, "rotation", t.eulerAngles());
  w |= writeScale(*this, t.localScale());
  if (!w)
    writeLine("// identity");
  endBlock();
}

inline void
SceneWriter::writeComponent(const Component* c)
{
  if (auto t = asTransform(c))
    writeTransform(*t);
  else if (auto function = _writeMap.find(*c))
    function(*this, *c);
}

void
SceneWriter::writeSceneObject(const SceneObject* object)
{
  beginBlock("object", object->name());
  for (auto& child : object->children())
    writeSceneObject(&child);
  for (auto component : object->components())
    writeComponent(component);
  endBlock();
}

inline void
SceneWriter::writeMaterial(const Material& m)
{
  beginBlock("material", m.name());
  writeColor("ambient", m.ambient);
  writeColor("diffuse", m.diffuse);
  writeColor("spot", m.spot);
  writeFloat("shine", m.shine);
  writeColor("specular", m.specular);
  writeColor("transparency", m.transparency);
  writeFloat("ior", m.ior);
  endBlock();
}

void
SceneWriter::write(const Scene& scene)
{
  for (const auto& [name, m] : Assets::materials())
    if (m->referenceCount() > 1)
      writeMaterial(*m);
  beginBlock("scene", scene.name());
  writeEnvironment(scene);
  for (auto& object : scene.root()->children())
    writeSceneObject(&object);
  endBlock();
}

SceneWriter::WriteMap::WriteMap()
{
  SceneWriter::registerWriteFunction(writeCamera);
  SceneWriter::registerWriteFunction(writeLight);
  SceneWriter::registerWriteFunction(writePrimitive);
}

} // end namespace cg::util
