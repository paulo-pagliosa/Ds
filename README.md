# Ds

Ds is an Open Source C++ class library for [OpenGL]-based graphics application
development. It contains classes and templates for math objects (vectors,
points, quaternions, and transformations), spatial data structures (grids,
trees, and bounding volume hierarchies), scene graph management ("[Unity]-like"
scene object hierarchy and components such as triangle meshes, lights, and
cameras), and basic [OpenGL] rendering and ray-tracing support, among others.

Ds has been employed in the teaching of subjects such as computer graphics,
visualization, and games, in undergraduate and graduate courses in Computer
Science at the [Faculty of Computing](https://www.facom.ufms.br) of the
[Federal University of Mato Grosso do Sul](https://www.ufms.br). Ds has
also been used in several research projects in physics-based animation and
geometric processing.

The current master version is 1.5 and supports Windows 11, Linux, and macOS.
Ds is written in C++20.

## Building Ds

All headers and source files are in the [`cg/`](cg) folder. The only
dependencies are [GLFW] 3.5 and [Dear ImGui]. All headers, source files,
and libraries (GLFW static libraries for Windows and macOS) needed are
already included in the [`cg/externals/`](cg/externals) folder (package
`glfw3` is required for Linux).

**Building with Visual Studio** (Windows)

Open [`cg/build/vs2022/cg.vcxproj`](cg/build/vs2022) and build it.

**Building with CMake** (≥ 3.18, Windows, Linux, and macOS)

From the `cg` folder, run the build script for your platform, optionally
passing the configuration (`Release`, the default, or `Debug`):
```bash
./build.sh [Release|Debug]   # Linux and macOS
build.bat [Release|Debug]    # Windows
```
The library is written to the `cg/lib/` folder: `cg.lib` (`cgD.lib` in
Debug) on Windows, and `libcg.a` (`libcgD.a` in Debug) on Linux and macOS.

The scripts are thin wrappers around CMake. To run it by hand, the same two
commands work on all platforms (each generator ignores the option that does
not apply to it):
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Platform notes:

- **Windows:** CMake uses your default toolchain (pass
  `-G "Visual Studio 17 2022"` to force it explicitly).
- **Linux:** install the `glfw3` development package through your distro's
  package manager first (GLFW itself is *not* vendored for Linux).
- **macOS:** install the Xcode Command Line Tools (`xcode-select --install`)
  and CMake. Apple deprecated OpenGL and supports it only up to version 4.1,
  which is what Ds requires; the deprecation warnings are silenced by the
  build.

**Using Ds in your own application**

With CMake, include [`cg/ImportCg.cmake`](cg/import.cmake) in your
`CMakeLists.txt` and link the imported target `cg`. It brings along Ds's
include folders, compile definitions, and dependencies (OpenGL, GLFW, and
platform-specific settings), and picks `cg` or `cgD` according to the
configuration:
```cmake
include(<path to cg>/ImportCg.cmake)
target_link_libraries(myapp PRIVATE cg)
```

## Ds-Vis

Ds-Vis is a simple "[VTK]-like" scientific visualization library extending Ds.
The API contains classes and templates for datasets and processes. A dataset
consists of a structure (geometry and topology defined by vertices and cells)
and attributes (e.g., scalars and/or vectors at vertices and cells). The API
provides classes for point sets, triangle meshes, triangle mesh instancing,
and sets of graphics primitives containing points, lines, and triangles.
Processes can be sources (dataset producers), filters (dataset transformers),
and sinks (dataset consumers). Processes are connected to each other in a
visualization pipeline such that the output of a source or filter is the input
of another filter or sink. A mapper is a type of sink capable of rendering its
input dataset. Every actor in a scene has a mapper.

### Building Ds-Vis

All headers and source files of Ds-Vis are in the [`cgvis/`](cgvis) folder.
Ds-Vis depends on Ds — build Ds first (see [Building Ds](#building-ds) above).

**Building with Visual Studio 2022** (Windows)

Open [`cgvis/build/vs2022/cgvis.vcxproj`](cgvis/build/vs2022) and build
  it.

**Building with CMake** (≥ 3.18, Windows, Linux, and macOS)

From the `cgvis` folder, run the build script for your platform, the same way
as for Ds:
```bash
./build.sh [Release|Debug]   # Linux and macOS
build.bat [Release|Debug]    # Windows
```
The library is written to the `cgvis/lib/` folder (`cgvis.lib`/`cgvisD.lib`
on Windows, `libcgvis.a`/`libcgvisD.a` on Linux and macOS). The manual
CMake commands and platform notes of [Building Ds](#building-ds) apply as
well.

Ds-Vis is a separate static library that does *not* embed Ds's object
code — anyone linking against Ds-Vis (a demo, for instance) must link
both `cgvis` and `cg` (plus their dependencies). With CMake, include
[`cgvis/ImportCgvis.cmake`](cgvis/ImportCgvis.cmake) and link the imported
target `cgvis`, which brings `cg` along:
```cmake
include(<path to cgvis>/import.cmake)
target_link_libraries(myapp PRIVATE cgvis)
```

## Demos

Ds ships with demo applications built on top of the library, each depending
on Ds or Ds-Vis and following the same build pattern used for the libraries
above. See the [demos page](docs/demos.md) for the full list, source folders,
and pre-built Windows binaries.

<div align="center">
<img style="width:90%;height:auto;" alt="cgdemo" src="https://github.com/user-attachments/assets/b6b552b5-91ed-461b-b5f1-9dedffd75b85" />

*Ds Demo, a ray-tracing application built on top of Ds. [See all demos →](docs/demos.md)*
</div>

## Upcoming Changes

Some upcoming goals are:

- Full API documentation
- ~~CMake project and support for Linux and macOS~~
- Textured and transparent materials
- ~~Basic scientific visualization API~~
- Demos for 2D/3D neighbor particle searching with regular grids and
  quadtrees/octrees
- Subdivision surfaces and T-splines rendering API

## Credits

Developed by [Paulo Pagliosa](https://www.facom.ufms.br/~pagliosa).<br/>
First version of CMake files by [Felipe Silva](https://github.com/MachSilva).

## License

Ds is licensed under the zlib License, see [LICENSE.txt](LICENSE.txt) for more
information.

## Contact

If you have questions related to the use of Ds, a bug to report or a
feature you would like to request, please send an e-mail to:
*ppagliosa @ gmail dot com*.

[//]:#

[OpenGL]: <https://www.opengl.org>
[Unity]: <https://unity.com>
[GLFW]: <https://www.glfw.org>
[Dear ImGui]: <https://github.com/ocornut/imgui>
[CUDA]: <https://developer.nvidia.com/cuda-downloads>
[VTK]: <https://vtk.org/>
