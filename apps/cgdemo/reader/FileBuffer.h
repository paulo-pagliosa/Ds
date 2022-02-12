//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2007, 2022 Paulo Pagliosa.                        |
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
// OVERVIEW: FileBuffer.h
// ========
// Class definition for file buffer.
//
// Author: Paulo Pagliosa
// Last revision: 07/02/2022

#ifndef __FileBuffer_h
#define __FileBuffer_h

#include "Buffer.h"
#include <filesystem>
#include <fstream>

namespace cg
{ // begin namespace cg

namespace fs = std::filesystem;

namespace parser
{ // begin namespace parser


/////////////////////////////////////////////////////////////////////
//
// FileBuffer: file buffer class
// ==========
class FileBuffer: public Buffer
{
public:
  FileBuffer(const fs::path& path);

  String name() const override;

  const auto& path() const
  {
    return _path;
  }

  const auto& file() const
  {
    return _file;
  }

private:
  fs::path _path;
  std::ifstream _file;
  size_t _size;

  char advance() override;

  void flush();
  void fill(char*, size_t = 0);

  auto endBuffer() const
  {
    return _begin + _size;
  }

}; // FileBuffer

} // end namespace parser

} // end namespace cg

#endif // __FileBuffer_h
