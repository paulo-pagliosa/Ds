//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2022 Paulo Pagliosa.                              |
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
// OVERVIEW: TriCellMeshNormals.cpp
// ========
// Source file for vis triangle mesh normal extractor.
//
// Author: Paulo Pagliosa
// Last revision: 16/03/2022

#include "TriCellMeshNormals.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// TriCellMeshNormals implementation
// ==================
void
TriCellMeshNormals::execute()
{
  // TODO: cell normal extraction
  auto output = Object::makeCopy(input());

  if (output == nullptr)
    throw std::runtime_error("TriCellMeshNormals::execute(): no output");
  this->setOutput(output);

  auto normals = output->mesh()->data().vertexNormals;

  if (normals == nullptr)
  {
    output->setVertexVectors(nullptr);
    return;
  }

  auto nv = output->vertexCount();
  auto vectors = output->vertexVectors();

  if (vectors == nullptr || nv != vectors->size())
  {
    vectors = new Vectors{nv};
    output->setVertexVectors(vectors);
  }

  const auto& normalMatrix = input()->normalMatrix();

  for (decltype(nv) i = 0; i < nv; ++i)
    vectors->set(i, normalMatrix * normals[i]);
}

} // end namespace cg::vis
