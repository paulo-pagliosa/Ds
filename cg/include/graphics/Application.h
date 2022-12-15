//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2022 Paulo Pagliosa.                        |
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
// Last revision: 14/12/2022

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
  /// Destructor.
  virtual ~Application();

  /// Constructs an instance of Application.
  Application(GLWindow* mainWindow);

  /// Returns the title of this application.
  auto id() const
  {
    return _id;
  }

  /// Runs this application.
  int run(int argc, char** argv);

  /// Returns the application base directory.
  static const auto& baseDirectory()
  {
    return _baseDirectory;
  }

  /// Returns the asset file path for \c filename.
  static std::string assetFilePath(const char* filename)
  {
    return _assetsPath + filename;
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
  GLWindow* _mainWindow;
  int _id;

  static std::string _baseDirectory;
  static std::string _assetsPath;
  static int _count;

}; // Application

} // end namespace cg

#endif // __Application_h
