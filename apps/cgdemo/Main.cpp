#include "graphics/Application.h"
#include "MainWindow.h"

int
main(int argc, char** argv)
{
  return cg::Application{new MainWindow{1280, 720}}.run(argc, argv);
}
