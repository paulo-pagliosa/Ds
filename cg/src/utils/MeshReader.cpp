//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2023 Paulo Pagliosa.                        |
//|                                                                 |
//| This software is provided 'as-is', without any express or       |
//| implied warranty. In no event will the authors be held liable   |
//| for any damages arising from the use of this software.          |
//|                                                                 |
//| Permission is granted to anyone to use this software for any    |
//| purpose, including commercial applications, and to alter it and |
//| redistribute it freely, subject to the following restrictions:  |
//|                                                                 |
//| 1. The origin of this software must not be misrepresented; you  |
//| must not claim that you wrote the original software. If you use |
//| this software in a product, an acknowledgment in the product    |
//| documentation would be appreciated but is not required.         |
//|                                                                 |
//| 2. Altered source versions must be plainly marked as such, and  |
//| must not be misrepresented as being the original software.      |
//|                                                                 |
//| 3. This notice may not be removed or altered from any source    |
//| distribution.                                                   |
//|                                                                 |
//[]---------------------------------------------------------------[]
//
// OVERVIEW: MeshReader.cpp
// ========
// Source file for OBJ mesh reader.
//
// Author: Paulo Pagliosa
// Last revision: 17/07/2023

#include "utils/MeshReader.h"
#include <filesystem>

namespace cg
{ // begin namespace cg

namespace
{ // begin namespace

void
readMeshSize(FILE* file, TriangleMesh::Data& data)
{
  constexpr auto maxSize = 256;
  int nv{};
  int nt{};

  char buffer[maxSize];

  while (char* line = fgets(buffer, maxSize, file))
    switch (*line++)
    {
      case 'v':
        if (*line == ' ')
          nv++;
        break;

      case 'f':
      {
        int v;
        int n;
        int t;
        int nfv{};

        for (;; ++nfv)
        {
          while (*line == ' ')
            line++;
          if (sscanf(line, "%d/%d/%d", &v, &t, &n) <= 0)
            break;
          while (*line && *line != ' ')
            ++line;
        }
        nt += nfv - 2;
        break;
      }
    }
  data.vertexCount = nv;
  data.triangleCount = nt;
}

void
readMeshData(FILE* file, TriangleMesh::Data& data)
{
  constexpr auto maxSize = 256;
  auto vertex = data.vertices;
  auto triangle = data.triangles;

  char buffer[maxSize];

  while (char* line = fgets(buffer, maxSize, file))
    switch (*line++)
    {
      case 'v':
        if (*line == ' ')
        {
          float x;
          float y;
          float z;

          (void)sscanf(line + 1, "%f %f %f", &x, &y, &z);
          vertex->set(x, y, z);
          vertex++;
        }
        break;

      case 'f':
      {
        int v;
        int n;
        int t;

        // This version reads vertex coordinates only and
        // ignores vertex texture coordinates and normals
        for (int i = 0;; ++i)
        {
          while (*line == ' ')
            line++;
          if (sscanf(line, "%d/%d/%d", &v, &t, &n) <= 0)
            break;
          if (i < 3)
            triangle->v[i] = v - 1;
          else
          {
            auto pt = triangle++;

            triangle->v[0] = pt->v[0];
            triangle->v[1] = pt->v[2];
            triangle->v[2] = v - 1;
          }
          while (*line && *line != ' ')
            ++line;
        }
        triangle++;
        break;
      }
    }
}

} // end namespace


/////////////////////////////////////////////////////////////////////
//
// MeshReader implementation
// ==========
TriangleMesh*
MeshReader::readOBJ(const char* filename)
{
  FILE* file = fopen(filename, "r");

  if (file == nullptr)
    return nullptr;

  TriangleMesh::Data data;

  readMeshSize(file, data);
  data.vertices = new vec3f[data.vertexCount];
  data.vertexNormals = nullptr;
  data.triangles = new TriangleMesh::Triangle[data.triangleCount];
  rewind(file);
  printf("Reading Wavefront OBJ file %s...\n", filename);
  readMeshData(file, data);
  fclose(file);

  auto mesh = new TriangleMesh{std::move(data)};

  mesh->computeNormals();
  return mesh;
}

} // end namespace cg
