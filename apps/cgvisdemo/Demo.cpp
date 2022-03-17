//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2022 Paulo Pagliosa.                              |
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
// OVERVIEW: Demo.cpp
// ========
// Source file for vis demo function.
//
// Author: Paulo Pagliosa
// Last revision: 16/03/2022

#include "graphics/Application.h"
#include "ContourFilter.h"
#include "ElevationFilter.h"
#include "Glyph3.h"
#include "PolyDataMapper.h"
#include "PolyMeshMapper.h"
#include "TransformFilter.h"
#include "TriCellMeshMapper.h"
#include "TriCellMeshNormals.h"
#include "TriCellMeshReader.h"
#include "SphereSource.h"
#include "Scene.h"
#include "imgui.h"
#include <fstream>
#include <sstream>
#include <utility>

namespace
{ // begin namespace

using namespace cg;
using namespace cg::vis;

std::pair<Actor*, Actor*>
scalarDemo(Source<TriCellMesh>* source, LookupTable* colorTable)
{
  auto elevationFilter = ElevationFilter<TriCellMesh>::New();

  elevationFilter->setInput(*source);
  elevationFilter->setDirection(ElevationDirection::Z);

  auto meshMapper = TriCellMeshMapper::New();

  meshMapper->setInput(*elevationFilter);
  meshMapper->setLookupTable(colorTable);

  auto contourFilter = ContourFilter<TriCellMesh>::New();

  contourFilter->setInput(*elevationFilter);
  contourFilter->generateValues(10, 0, 1);

  auto isolineMapper = PolyDataMapper::New();

  isolineMapper->setInput(*contourFilter);
  isolineMapper->setLookupTable(colorTable);
  return {new Actor(*meshMapper), new Actor(*isolineMapper)};
}

Actor*
vectorDemo(Source<TriCellMesh>* source,
  LookupTable* colorTable,
  GlyphScaleMode scaleMode)
{
  auto elevationFilter = ElevationFilter<TriCellMesh>::New();

  elevationFilter->setInput(*source);
  elevationFilter->setDirection(ElevationDirection::Z);

  auto normals = TriCellMeshNormals::New();

  normals->setInput(*elevationFilter);

  auto glyphs = Glyph3<TriCellMesh>::New();

  glyphs->setInput(*normals);
  glyphs->setScaleMode(scaleMode);
  glyphs->setScaleFactor(0.7f);

  auto glyphMapper = PolyMeshMapper::New();

  glyphMapper->setInput(*glyphs);
  glyphMapper->setLookupTable(colorTable);
  return new Actor{*glyphMapper};
}

} // end namespace

void
demo(cg::vis::Scene& scene)
{
  using namespace cg::vis;

  try
  {
    {
      auto colorTable = LookupTable::jet();
      auto sphere = SphereSource::New();

      sphere->setResolution(20);

      auto [a1, a2] = scalarDemo(sphere, colorTable);
      auto a3 = vectorDemo(sphere, colorTable, GlyphScaleMode::Vector);

      scene.addActor(a1, "Sphere");
      scene.addActor(a2, "Sphere Isolines");
      scene.addActor(a3, "Sphere Normals");

      auto ellipsoid = TransformFilter<TriCellMesh>::New();

      ellipsoid->setInput(*sphere);
      ellipsoid->setPosition(vec3f{3, 0, 0});
      ellipsoid->setScale(vec3f{0.5f, 1.5f, 2});

      auto [a4, a5] = scalarDemo(ellipsoid, colorTable);
      auto a6 = vectorDemo(ellipsoid, colorTable, GlyphScaleMode::Scalar);

      scene.addActor(a4, "Ellipsoid");
      scene.addActor(a5, "Ellipsoid Isolines");
      scene.addActor(a6, "Ellipsoid Normals");
    }
    {
      auto colorTable = LookupTable::cooper();
      auto reader = TriCellMeshReader::New();

      reader->setPath(Application::baseDirectory() +
        "../cgdemo/assets/meshes/f-16.obj");

      auto [a1, a2] = scalarDemo(reader, colorTable);

      scene.addActor(a1, "F-16");
      scene.addActor(a2, "F-16 Isolines");
    }

    auto light = new Light;

    light->setType(Light::Type::Directional);
    light->setEulerAngles({50, 30, 0});
    scene.addLight(light, "Main Light");
    scene.ambientLight = Color::gray;
  }
  catch (const std::exception& e)
  {
    std::cout << "Error: " << e.what() << '\n';
  }
}

void
demoLog()
{
  static std::string demoCode;

  if (demoCode.empty())
  {
    std::ifstream f(Application::baseDirectory() + "demo.txt");

    if (!f)
      throw std::runtime_error("Demo code file not found");
#ifdef _DEBUG
    puts("Reading demo.txt...");
#endif // _DEBUG

    std::ostringstream s;
    
    s << f.rdbuf();
    demoCode = s.str();
  }
  ImGui::TextUnformatted(demoCode.c_str());
}
