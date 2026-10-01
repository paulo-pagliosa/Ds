#!/usr/bin/env bash
# Builds the cgvis library on macOS and Linux.
# Usage: ./build.sh [Release|Debug]   (default: Release)

set -e

CONFIG="${1:-Release}"

case "$CONFIG" in
  Release|Debug) ;;
  *)
    echo "Usage: $0 [Release|Debug]"
    exit 1
    ;;
esac

# Run from the script's folder, wherever it is called from
cd "$(dirname "$0")"

# Single-config generator: one build folder per configuration
cmake -S . -B "build/$CONFIG" -DCMAKE_BUILD_TYPE="$CONFIG"
cmake --build "build/$CONFIG" --parallel
