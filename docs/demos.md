# Demos

Ds ships with a growing set of demo applications built on top of the library.
Every demo follows the same [build pattern](../README.md#building): a Visual
Studio 2022 solution/project under `build/vs2022/`, and a `CMakeLists.txt` for
Linux, inside the demo's own folder under `apps/`.

Each demo has its own version number, independent of the version of the
library (Ds or Ds-Vis) it depends on — so a release of the library may ship
demo binaries at different version numbers.

Demos don't have build dependencies of their own — they only need the
corresponding library (Ds or Ds-Vis) already built, following the
[Building](../README.md#building) instructions.

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

[![cgdemo](https://user-images.githubusercontent.com/32277980/153728061-df2b3644-176b-44cc-bbc0-ba3011012ee0.png)](https://user-images.githubusercontent.com/32277980/153728061-df2b3644-176b-44cc-bbc0-ba3011012ee0.png)

The ray tracing relies on bounding volume hierarchies (BVHs) for accelerating
ray/object intersections. Also, the code includes a scene reader with a LL(1)
parser. The grammar for the specification of a scene and its scene objects is
[here](../apps/cgdemo/reader/grammar.txt). The scene file shown in this
example is available [here](../apps/cgdemo/assets/scenes).

[![cgdemo-scene](https://user-images.githubusercontent.com/32277980/153728083-cd875b86-2d29-41d2-8b01-b2a610d5e7d1.png)](https://user-images.githubusercontent.com/32277980/153728083-cd875b86-2d29-41d2-8b01-b2a610d5e7d1.png)

## Ds-Vis Demo

Ds-Vis Demo is a visualization application built using Ds and Ds-Vis. Headers
and source files are in [`apps/cgvisdemo/`](../apps/cgvisdemo). It reuses an
asset from Ds Demo's assets folder, so its `apps/cgvisdemo/` and Ds Demo's
`apps/cgdemo/` folders must sit side by side under the same parent
directory — this applies to the source tree and to the pre-built binaries
alike (see the note above).

[Download the latest Windows binary](https://github.com/paulo-pagliosa/Ds/releases/download/v1.5/cgvisdemo-win64-v1.0.zip)

[![cgvisdemo](https://user-images.githubusercontent.com/32277980/158721233-25ec422a-9922-4b92-89d2-e5a41376d25d.png)](https://user-images.githubusercontent.com/32277980/158721233-25ec422a-9922-4b92-89d2-e5a41376d25d.png)