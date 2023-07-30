//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2007, 2023 Paulo Pagliosa.                        |
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
// OVERVIEW: FileBuffer.cpp
// ========
// Source file for file buffer.
//
// Author: Paulo Pagliosa
// Last revision: 30/07/2023

#include "FileBuffer.h"
#include <cassert>
#include <cstring>

namespace cg::parser
{ // begin namespace cg::parser

namespace
{ // begin namespace

constexpr size_t maxFileSize{0x4000};
constexpr size_t maxLexemeSize{1024};
constexpr size_t maxLook{16};
constexpr size_t dflBufferSize{maxLexemeSize * 3 + maxLook * 2};

} // end namespace


/////////////////////////////////////////////////////////////////////
//
// FileBuffer implementation
// ==========
FileBuffer::FileBuffer(const fs::path& path):
  _path{path},
  _size{}
{
  assert(fs::is_regular_file(path));
  _file.open(path, std::ios::in | std::ios::binary);
  if (!_file.is_open())
    return;
  if ((_size = (size_t)fs::file_size(path) + 1) > maxFileSize)
    _size = dflBufferSize;
  if ((_begin = new char[_size]) == nullptr)
    throw std::runtime_error("No memory for file buffer");
  *(_current = _end = _begin) = 0;
  fill(_begin, _size);
}

String
FileBuffer::name() const
{
  return _path.string();
}

char
FileBuffer::advance()
{
  if (_eofRead)
  {
    if (_current >= _end)
      return 0;
  }
  else if ((_end - maxLook) < _current)
    flush();
  return *++_current;
}

void
FileBuffer::flush()
{
  auto lftEdge = nullptr != _lexemeBegin  ? _lexemeBegin : _current;
  auto shlSize = (size_t)(lftEdge - _begin);

  if (shlSize < maxLexemeSize)
    throw std::runtime_error("File buffer is full");

  auto copySize = (size_t)(_end - lftEdge);

  if (copySize != 0)
    memmove(_begin, lftEdge, copySize);
  fill(_begin + copySize);
  _current -= shlSize;
  if (nullptr != _lexemeBegin)
    _lexemeBegin -= shlSize;
}

void
FileBuffer::fill(char* from, size_t size)
{
  if (size == 0)
    size = ((endBuffer() - from) / maxLexemeSize) * maxLexemeSize;
  if (size > 0)
  {
    _file.read(from, size);
    if (auto count = (size_t)_file.gcount(); count <= 0)
      throw std::runtime_error("Input file read error");
    else
    {
      _end = from + count;
      if (_file.eof())
      {
        _eofRead = true;
        *_end = 0;
      }
    }
  }
}

} // end namespace cg::parser
