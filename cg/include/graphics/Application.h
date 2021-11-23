//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018 Orthrus Group.                               |
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
// OVERVIEW: Application.h
// ========
// Class definition for graphics application.
//
// Author: Paulo Pagliosa
// Last revision: 06/08/2018

#ifndef __Application_h
#define __Application_h

#include "graphics/GLWindow.h"
#include "utils/MeshReader.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Application: graphics application class
// ===========
class Application
{
public:
  /// Constructs an instance of Application.
  Application(GLWindow* mainWindow);

  /// Destructor.
  virtual ~Application();

  /// Returns the title of this application.
  auto id() const
  {
    return _id;
  }

  /// Runs this application.
  int run(int argc, char** argv);

  /// Throws a runtime error exception.
  static void error(const char* format, ...);

  /// Returns the asset file path for \c filename.
  static std::string assetFilePath(const char* filename)
  {
    return _assetDir + filename;
  }

  /// Loads shaders from files \c vs and \c fs into \c p.
  static void loadShaders(GLSL::Program& p, const char* vs, const char* fs)
  {
    p.loadShaders(assetFilePath(vs), assetFilePath(fs));
  }

  /// Loads a mesh from an OBJ file.
  static TriangleMesh* loadMesh(const char* filename)
  {
    return MeshReader::readOBJ(assetFilePath(filename).c_str());
  }

private:
  int _id;
  GLWindow *_mainWindow;

  static std::string _assetDir;
  static int _count;

}; // Application

} // end namespace cg

#endif // __Application_h
