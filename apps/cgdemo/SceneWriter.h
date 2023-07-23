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
// OVERVIEW: SceneWriter.h
// ========
// Class definition for scene writer.
//
// Author: Paulo Pagliosa
// Last revision: 19/07/2023

#ifndef __SceneWriter_h
#define __SceneWriter_h

#include "graph/Scene.h"
#include "Writer.h"
#include <typeinfo>
#include <unordered_map>

namespace cg::util
{ // begin namespace cg::util

using namespace cg::graph;


/////////////////////////////////////////////////////////////////////
//
// SceneWriter: scene writer class
// ===========
class SceneWriter: public Writer
{
public:
  template <typename C = Component>
  using WriteFunction = void (*)(SceneWriter&, const C&);

  template <typename C>
  static void registerWriteFunction(WriteFunction<C> function)
  {
    assert(function != nullptr);
    _writeMap.add(function);
  }

  using Writer::Writer;
  using Writer::beginBlock;
  using Writer::write;

  virtual void write(const Scene& scene);

  template <typename... Args>
  void writeLine(const char* label, Args&&... args)
  {
    beginLine();
    write(label, std::forward<Args>(args)...);
    endLine();
  }

  void beginBlock(const char* label, const char* name = nullptr)
  {
    writeName(label, name);
    beginBlock();
  }

  void writeColor(const char* label, const Color& color)
  {
    writeLine("%s rgb(%g,%g,%g)", label, color.r, color.g, color.b);
  }

  template <typename real>
  void writeFloat(const char* label, real f)
  {
    writeLine("%s %g", label, f);
  }

  template <typename V>
  void writeVec3(const char* label, const V& v)
  {
    writeLine("%s <%g,%g,%g>", label, v.x, v.y, v.z);
  }

  template <typename V>
  void writeVec2(const char* label, const V& v)
  {
    writeLine("%s vec2(%g,%g)", label, v.x, v.y);
  }

  void writeName(const char* label, const char* name);

private:
  class WriteMap
  {
  public:
    WriteMap();

    template <typename C>
    void add(WriteFunction<C> function)
    {
      _functions[typeid(C).hash_code()] = (WriteFunction<>)function;
    }

    WriteFunction<> find(const Component& c) const
    {
      auto r = _functions.find(typeid(c).hash_code());
      return r != _functions.end() ? r->second : nullptr;
    }

  private:
    std::unordered_map<size_t, WriteFunction<>> _functions;

  }; // WriteMap

  inline static WriteMap _writeMap;

  void writeMaterial(const Material&);
  void writeEnvironment(const Scene&);
  void writeSceneObject(const SceneObject*);
  void writeComponent(const Component*);
  void writeTransform(const Transform&);

}; // SceneWriter

} // end namespace cg::util

#endif // __SceneWriter_h
