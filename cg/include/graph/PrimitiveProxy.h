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
// OVERVIEW: PrimitiveProxy.h
// ========
// Class definition for primitive proxy.
//
// Author: Paulo Pagliosa
// Last revision: 19/07/2023

#ifndef __PrimitiveProxy_h
#define __PrimitiveProxy_h

#include "graph/ComponentProxy.h"
#include "graphics/Actor.h"
#include "graphics/TriangleMeshMapper.h"

namespace cg::graph
{ // begin namespace cg::graph


/////////////////////////////////////////////////////////////////////
//
// PrimitiveProxy: primitive proxy class
// ==============
class PrimitiveProxy: public ComponentProxy<PrimitiveMapper>
{
public:
  static auto New(const PrimitiveMapper& mapper)
  {
    return new PrimitiveProxy{mapper};
  }

  PrimitiveMapper* mapper() const
  {
    return _object;
  }

  Actor* actor() const
  {
    return _actor;
  }

protected:
  Reference<Actor> _actor;

  PrimitiveProxy(const PrimitiveMapper& mapper):
    ComponentProxy<PrimitiveMapper>{"Primitive", mapper}
  {
    // do nothing
  }

  void afterAdded() override;
  void beforeRemoved() override;
  void transformChanged() override;
  void setVisible(bool value) override;

}; // PrimitiveProxy

inline auto
asPrimitive(const Component* component)
{
  return dynamic_cast<const PrimitiveProxy*>(component);
}


/////////////////////////////////////////////////////////////////////
//
// TriangleMeshProxy: triangle mesh proxy class
// =================
class TriangleMeshProxy final: public PrimitiveProxy
{
public:
  static auto New(const TriangleMesh& mesh, const std::string& meshName)
  {
    return new TriangleMeshProxy{mesh, meshName};
  }

  const char* const meshName() const
  {
    return _meshName.c_str();
  }

  void setMesh(const TriangleMesh& mesh, const std::string& meshName)
  {
    ((TriangleMeshMapper*)PrimitiveProxy::mapper())->setMesh(mesh);
    _meshName = meshName;
  }

private:
  std::string _meshName;

  TriangleMeshProxy(const TriangleMesh& mesh, const std::string& meshName):
    PrimitiveProxy{*new TriangleMeshMapper{mesh}},
    _meshName{meshName}
  {
    // do nothing
  }

}; // TriangleMeshProxy

} // end namepace cg::graph

#endif // __PrimitiveProxy_h
