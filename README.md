# Ds

Ds is an Open Source C++ class library for [OpenGL]-based graphics application
development. It contains classes and templates for math objects (vectors,
points, quaternions, and transformations), spatial data structures (grids,
trees, and bounding volume hierarchies), scene graph management ([Unity]-like
scene object hierarchy and components such as triangle meshes, lights, and
cameras), and basic [OpenGL] rendering and ray-tracing support, among others.

Ds has been employed in the teaching of subjects such as computer graphics,
visualization, and games, in undergraduate and graduate courses of Computer
Science at the [Faculty of Computing](https://www.facom.ufms.br) of the
[Federal University of Mato Grosso do Sul](https://www.ufms.br). Ds has
also been used in several research projects in physics-based animation
and geometric processing.

## Compiling Ds

The current master version is 1.0 and supports Windows 10 and later. All
headers and source files are in the [cg/](/cg) folder. The only dependencies
are [GLFW] and [Dear ImGui]. All headers, source files, and libraries needed
are already included in the [cg/externals/](/cg/externals) folder.
There are solution and project files for Visual Studio 2019 located in the
[cg/build/vs2019/](/cg/build/vs2019) folder. A pre-compiled binary is available
in the [cg/lib/](/cg/lib) folder.

## Demo


## Upcoming Changes

Some of the goals for 2022 are:

- Full API documentation
- CMake project and support for macOS and Linux with Clang and GCC
- Textured ans transparent materials
- Basic scientific visualization API
- Demos for 2D/3D neighbor particle searching with regular grids and
quadtrees/octress
- Bézier and B-spline curves and surfaces rendering API

## Credits

Developed by [Paulo Pagliosa](https://www.facom.ufms.br/~pagliosa).

## License

Ds is licensed under the zlib License, see [LICENCE.txt](/LICENSE.txt)
for more information.

## Contact
If you have questions related to the use of Ds, a bug to report or a
feature you would like to request, please send an e-mail to:<br/>
*ppagliosa @ gmail dot com*

[//]:#

[OpenGL]: <https://www.opengl.org>
[Unity]: <https://unity.com>
[GLFW]: <https://www.glfw.org>
[Dear ImGui]: <https://github.com/ocornut/imgui>
[CUDA]: <https://developer.nvidia.com/cuda-downloads>
