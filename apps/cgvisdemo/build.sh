#!/usr/bin/env bash
# Builds cgvisdemo on macOS and Linux.
# Usage: ./build.sh [Release|Debug]   (default: Release)
# The cg and cgvis libraries must be built first, in the same configuration.

set -e

CONFIG="${1:-Release}"

case "$CONFIG" in
  Release) SUFFIX= ;;
  Debug)   SUFFIX=D ;;
  *)
    echo "Usage: $0 [Release|Debug]"
    exit 1
    ;;
esac

# Run from the script's folder, wherever it is called from
cd "$(dirname "$0")"

# Fail early if a library has not been built yet
for LIB in "../../cg/lib/libcg$SUFFIX.a" "../../cgvis/lib/libcgvis$SUFFIX.a"; do
  if [ ! -f "$LIB" ]; then
    echo "Error: $LIB not found. Build it ($CONFIG) first."
    exit 1
  fi
done

# Single-config generator: one build folder per configuration
cmake -S . -B "build/$CONFIG" -DCMAKE_BUILD_TYPE="$CONFIG"
cmake --build "build/$CONFIG" --parallel
