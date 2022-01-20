#ifndef __MainWindow_h
#define __MainWindow_h

#include "graph/SceneWindow.h"
#include "graphics/Assets.h"
#include "graphics/BVH.h"
#include "graphics/GLImage.h"
#include "RayTracer.h"

using namespace cg;
using namespace cg::graph;

class MainWindow final: public SceneWindow
{
public:
  MainWindow(int width, int height):
    SceneWindow{"Ds Demo", width, height}
  {
    // do nothing
  }

private:
  using BVHRef = Reference<BVH>;
  using BVHMap = std::map<TriangleMesh*, BVHRef>;

  Reference<RayTracer> _rayTracer;
  Reference<GLImage> _image;
  BVHMap bvhMap;

  static MeshMap _defaultMeshes;

  auto makeDefaultPrimitive(const char* const meshName)
  {
    return makePrimitive(*_defaultMeshes[meshName], meshName);
  }

  void beginInitialize() override;
  void initializeScene() override;
  void renderScene() override;
  void createObjectMenu() override;
  bool onResize(int width, int height) override;
  void gui() override;

  void mainMenu();
  void fileMenu();
  void showOptions();

  //void drawPrimitive(Primitive&);
  void drawLight(Light&);
  void drawCamera(Camera&);

  static void buildDefaultMeshes();

}; // MainWindow

#endif // __MainWindow_h
