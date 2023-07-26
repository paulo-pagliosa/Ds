# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/rafael/Code/Ds/build/_deps/gl3w-src"
  "/home/rafael/Code/Ds/build/_deps/gl3w-build"
  "/home/rafael/Code/Ds/build/_deps/gl3w-subbuild/gl3w-populate-prefix"
  "/home/rafael/Code/Ds/build/_deps/gl3w-subbuild/gl3w-populate-prefix/tmp"
  "/home/rafael/Code/Ds/build/_deps/gl3w-subbuild/gl3w-populate-prefix/src/gl3w-populate-stamp"
  "/home/rafael/Code/Ds/build/_deps/gl3w-subbuild/gl3w-populate-prefix/src"
  "/home/rafael/Code/Ds/build/_deps/gl3w-subbuild/gl3w-populate-prefix/src/gl3w-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/rafael/Code/Ds/build/_deps/gl3w-subbuild/gl3w-populate-prefix/src/gl3w-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/rafael/Code/Ds/build/_deps/gl3w-subbuild/gl3w-populate-prefix/src/gl3w-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
