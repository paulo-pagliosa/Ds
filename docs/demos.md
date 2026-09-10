# Demos

Ds ships with a growing set of demo applications built on top of the library.
Every demo follows the same [build pattern](../README.md#building-ds): a Visual
Studio 2022 project (and solution) under `build/vs2022/` (for Windows), and a
`CMakeLists.txt`, inside the demo's own folder under `apps/`.

Each demo has its own version number, independent of the version of the
library (Ds or Ds-Vis) it depends on — so a release of the library may ship
demo binaries at different version numbers.

Demos don't have build dependencies of their own — they only need the
corresponding library (Ds or Ds-Vis) already built, following the
[Building Ds](../README.md#building-ds) and [Building Ds-Vis](../README.md#building-ds-vis) instructions.

> **Note:** Ds-Vis Demo reuses an asset from Ds Demo's assets folder. If
> you're downloading the pre-built Windows binaries below, extract both
> `cgdemo` and `cgvisdemo` as sibling folders under the same parent
> directory — the same layout they have in the source tree
> (`apps/cgdemo/`, `apps/cgvisdemo/`) — otherwise Ds-Vis Demo won't find
> the shared asset.

## Ds Demo

Ds Demo is a simple ray-tracing application built on top of Ds. Headers and
source files are in [`apps/cgdemo/`](../apps/cgdemo).

[Download the latest Windows binary](https://github.com/paulo-pagliosa/Ds/releases/download/v1.5/cgdemo-win64-v1.5.zip)

<img alt="cgdemo" src="https://github.com/user-attachments/assets/b6b552b5-91ed-461b-b5f1-9dedffd75b85" /></br>

The ray tracing relies on bounding volume hierarchies (BVHs) for accelerating
ray/object intersections. Also, the code includes a scene reader with a LL(1)
parser. The grammar for the specification of a scene and its scene objects is
[here](../apps/cgdemo/reader/grammar.txt). The scene file shown in this
example is available [here](../apps/cgdemo/assets/scenes).

<img alt="cgdemo-scene" src="https://github.com/user-attachments/assets/512c344f-1d20-498d-8d6a-862cab8c5561" />

## Ds-Vis Demo

Ds-Vis Demo is a visualization application built using Ds and Ds-Vis. Headers
and source files are in [`apps/cgvisdemo/`](../apps/cgvisdemo). It reuses an
asset from Ds Demo's assets folder, so its `apps/cgvisdemo/` and Ds Demo's
`apps/cgdemo/` folders must sit side by side under the same parent
directory — this applies to the source tree and to the pre-built binaries
alike (see the note above).

[Download the latest Windows binary](https://github.com/paulo-pagliosa/Ds/releases/download/v1.5/cgvisdemo-win64-v1.0.zip)

[![cgvisdemo](https://user-images.githubusercontent.com/32277980/158721233-25ec422a-9922-4b92-89d2-e5a41376d25d.png)](https://user-images.githubusercontent.com/32277980/158721233-25ec422a-9922-4b92-89d2-e5a41376d25d.png)
