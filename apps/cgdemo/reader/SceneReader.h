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
// OVERVIEW: SceneReader.h
// ========
// Class definition for scene reader.
//
// Author: Paulo Pagliosa
// Last revision: 07/02/2022

#ifndef __SceneReader_h
#define __SceneReader_h

#include "graph/SceneObjectBuilder.h"
#include "graphics/Assets.h"
#include "ReaderBase.h"

namespace cg::parser
{ // begin namespace cg::parser


/////////////////////////////////////////////////////////////////////
//
// SceneReader: scene reader class
// ===========
class SceneReader: public Reader
{
public:
  class Parser;

  MaterialMap materials;

  graph::Scene* scene() const
  {
    return _scene;
  }

  Material* findMaterial(const String& name) const
  {
    if (auto mit = materials.find(name); mit != materials.end())
      return mit->second;
    return nullptr;
  }

  void execute() override;

private:
  Reference<graph::Scene> _scene;

  Reader::Parser* makeParser() override;

  friend Parser;

}; // SceneReader


/////////////////////////////////////////////////////////////////////
//
// SceneReader::Parser: scene reader parser class
// ===================
class SceneReader::Parser: public Reader::Parser,
  public graph::SceneObjectBuilder
{
public:
  Parser(SceneReader& reader):
    Reader::Parser{reader},
    _reader{&reader}
  {
    // do nothing
  }

  auto& reader() const
  {
    return *_reader;
  }

private:
  enum
  {
    _CAMERA,
    _LIGHT,
    _BOX,
    _SPHERE,
    _MESH
  };

  // Scene tokens
  enum
  {
    _AMBIENT = Reader::Parser::lastToken,
    _ANGLE,
    _ASPECT,
    _BACKGROUND,
    _COLOR,
    _COMPONENT,
    _DEPTH,
    _DIFFUSE,
    _DIRECTIONAL,
    _ENVIRONMENT,
    _FALLOFF,
    _FINISH,
    _HEIGHT,
    _MATERIAL,
    _OBJECT,
    _PARALLEL,
    _PERSPECTIVE,
    _POINT,
    _POSITION,
    _RANGE,
    _ROTATION,
    _SCALE,
    _SCENE,
    _SHINE,
    _SPECULAR,
    _SPOT,
    _TRANSFORM
  };

  // Errors
  enum
  {
    MATERIAL_ALREADY_DEFINED = Reader::Parser::lastErrorCode,
    COLOR_EXPECTED,
    MULTIPLE_SCENE_DEFINITION,
    COMPONENT_ALREADY_DEFINED,
    INVALID_VALUE_FOR,
    COULD_NOT_FIND_MESH,
    COULD_NOT_FIND_MATERIAL,
    lastErrorCode
  };

  struct Finish
  {
    float ambient{0.2f};
    float diffuse{0.8f};
    float spot{1};
    float shine{100};
    float specular{1};

  }; // Finish

  SceneReader* _reader;

  void start() override;

  void preamble();
  void declaration();

  auto initializer()
  {
    return expression();
  }

  void parseScene();
  void parseSceneEnvironment();
  void parseMaterial();
  void parseSurface(Material&, const Color&);
  void parseObject(graph::SceneObject&);
  void parseObjectBlock(graph::SceneObject&);
  void parseChildObjectBlock(graph::SceneObject&);
  void parseTransform(graph::SceneObject&);
  void parseComponent(graph::SceneObject&);

  Reference<graph::CameraProxy> matchCamera();
  Reference<graph::LightProxy> matchLight();
  Reference<graph::PrimitiveProxy> matchPrimitive(int);

  DECLARE_KEYWORD_TABLE(SceneReader::Parser);
  DECLARE_ERROR_MESSAGE_TABLE(SceneReader::Parser);

}; // SceneReader::Parser

} // end namespace cg::parser

#endif // __SceneReader_h
