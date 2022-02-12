//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2022 Paulo Pagliosa.                        |
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
// Last revision: 10/02/2022

#include "utils/MeshReader.h"
#include <filesystem>

namespace cg
{ // begin namespace cg

namespace internal
{ // begin namespace internal

void
readMeshSize(FILE* file, TriangleMesh::Data& data)
{
  constexpr auto lineSize = 128;
  int nv{};
  int nt{};

  for (char line[lineSize]; fscanf_s(file, "%s", line, lineSize) != EOF;)
    switch (line[0])
    {
      case 'v':
        if (line[1] == '\0')
            nv++;
        fgets(line, lineSize, file);
        break;

      case 'f':
      {
        int v;
        int n;
        int t;

        fscanf_s(file, "%s", line, lineSize);
        /* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
        if (strstr(line, "//"))
        {
          /* v//n */
          sscanf_s(line, "%d//%d", &v, &n);
          fscanf_s(file, "%d//%d", &v, &n);
          fscanf_s(file, "%d//%d", &v, &n);
          nt++;
          while (fscanf_s(file, "%d//%d", &v, &n) > 0)
            nt++;
        }
        else if (sscanf_s(line, "%d/%d/%d", &v, &t, &n) == 3)
        {
          /* v/t/n */
          fscanf_s(file, "%d/%d/%d", &v, &t, &n);
          fscanf_s(file, "%d/%d/%d", &v, &t, &n);
          nt++;
          while (fscanf_s(file, "%d/%d/%d", &v, &t, &n) > 0)
            nt++;
        }
        else if (sscanf_s(line, "%d/%d", &v, &t) == 2)
        {
          /* v/t */
          fscanf_s(file, "%d/%d", &v, &t);
          fscanf_s(file, "%d/%d", &v, &t);
          nt++;
          while (fscanf_s(file, "%d/%d", &v, &t) > 0)
            nt++;
        }
        else
        {
          /* v */
          fscanf_s(file, "%d", &v);
          fscanf_s(file, "%d", &v);
          nt++;
          while (fscanf_s(file, "%d", &v) > 0)
            nt++;
        }
        break;
      }

      default:
        fgets(line, lineSize, file);
    }
  data.vertexCount = nv;
  data.triangleCount = nt;
}

void
readMeshData(FILE* file, TriangleMesh::Data& data)
{
  constexpr auto lineSize = 128;
  auto vertex = data.vertices;
  auto triangle = data.triangles;

  for (char line[lineSize]; fscanf_s(file, "%s", line, lineSize) != EOF;)
    switch (line[0])
    {
      case 'v':
      {
        float x;
        float y;
        float z;

        switch (line[1])
        {
          case '\0':
            fscanf_s(file, "%f %f %f", &x, &y, &z);
            vertex->set(x, y, z);
            vertex++;
            break;

          default:
            fgets(line, lineSize, file);
        }
        break;
      }

      case 'f':
      {
        int v;
        int n;
        int t;

        fscanf_s(file, "%s", line, lineSize);
        /* Can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
        if (strstr(line, "//"))
        {
          /* v//n */
          sscanf_s(line, "%d//%d", &v, &n);
          triangle->v[0] = v - 1;
          fscanf_s(file, "%d//%d", &v, &n);
          triangle->v[1] = v - 1;
          fscanf_s(file, "%d//%d", &v, &n);
          triangle->v[2] = v - 1;
          triangle++;
          while (fscanf_s(file, "%d//%d", &v, &n) > 0)
          {
            triangle->v[0] = triangle[-1].v[0];
            triangle->v[1] = triangle[-1].v[2];
            triangle->v[2] = v - 1;
            triangle++;
          }
        }
        else if (sscanf_s(line, "%d/%d/%d", &v, &t, &n) == 3)
        {
          /* v/t/n */
          triangle->v[0] = v - 1;
          fscanf_s(file, "%d/%d/%d", &v, &t, &n);
          triangle->v[1] = v - 1;
          fscanf_s(file, "%d/%d/%d", &v, &t, &n);
          triangle->v[2] = v - 1;
          triangle++;
          while (fscanf_s(file, "%d/%d/%d", &v, &t, &n) > 0)
          {
            triangle->v[0] = triangle[-1].v[0];
            triangle->v[1] = triangle[-1].v[2];
            triangle->v[2] = v - 1;
            triangle++;
          }
        }
        else if (sscanf_s(line, "%d/%d", &v, &t) == 2)
        {
          /* v/t */
          triangle->v[0] = v - 1;
          fscanf_s(file, "%d/%d", &v, &t);
          triangle->v[1] = v - 1;
          fscanf_s(file, "%d/%d", &v, &t);
          triangle->v[2] = v - 1;
          triangle++;
          while (fscanf_s(file, "%d/%d", &v, &t) > 0)
          {
            triangle->v[0] = triangle[-1].v[0];
            triangle->v[1] = triangle[-1].v[2];
            triangle->v[2] = v - 1;
            triangle++;
          }
        }
        else
        {
          /* v */
          sscanf_s(line, "%d", &v);
          triangle->v[0] = v - 1;
          fscanf_s(file, "%d", &v);
          triangle->v[1] = v - 1;
          fscanf_s(file, "%d", &v);
          triangle->v[2] = v - 1;
          triangle++;
          while (fscanf_s(file, "%d", &v) > 0)
          {
            triangle->v[0] = triangle[-1].v[0];
            triangle->v[1] = triangle[-1].v[2];
            triangle->v[2] = v - 1;
            triangle++;
          }
        }
        break;
      }

      default:
        fgets(line, lineSize, file);
    }
}

} // end namespace internal


/////////////////////////////////////////////////////////////////////
//
// MeshReader implementation
// ==========
TriangleMesh*
MeshReader::readOBJ(const char* filename)
{
  FILE* file;

  fopen_s(&file, filename, "r");
  if (file == nullptr)
    return nullptr;

  TriangleMesh::Data data;

  internal::readMeshSize(file, data);
  data.vertices = new vec3f[data.vertexCount];
  data.vertexNormals = nullptr;
  data.triangles = new TriangleMesh::Triangle[data.triangleCount];
  rewind(file);
  printf("Reading Wavefront OBJ file %s...\n", filename);
  internal::readMeshData(file, data);
  fclose(file);

  auto mesh = new TriangleMesh{std::move(data)};

  mesh->computeNormals();
  return mesh;
}

} // end namespace cg
