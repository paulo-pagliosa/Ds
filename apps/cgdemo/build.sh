#!/usr/bin/env bash
# Builds cgdemo on macOS and Linux.
# Usage: ./build.sh [Release|Debug]   (default: Release)
# The cg library must be built first, in the same configuration.

set -e

CONFIG="${1:-Release}"

case "$CONFIG" in
  Release) CG_LIB=libcg.a ;;
  Debug)   CG_LIB=libcgD.a ;;
  *)
    echo "Usage: $0 [Release|Debug]"
    exit 1
    ;;
esac

# Run from the script's folder, wherever it is called from
cd "$(dirname "$0")"

# Fail early if the cg library has not been built yet
if [ ! -f "../../cg/lib/$CG_LIB" ]; then
  echo "Error: ../../cg/lib/$CG_LIB not found. Build cg ($CONFIG) first."
  exit 1
fi

# Single-config generator: one build folder per configuration
cmake -S . -B "build/$CONFIG" -DCMAKE_BUILD_TYPE="$CONFIG"
cmake --build "build/$CONFIG" --parallel
