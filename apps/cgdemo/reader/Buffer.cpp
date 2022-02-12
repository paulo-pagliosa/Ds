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
// OVERVIEW: Buffer.cpp
// ========
// Source file for generic input buffer.
//
// Author: Paulo Pagliosa
// Last revision: 07/02/2022

#include "Buffer.h"

namespace cg::parser
{ // begin namespace cg::parser


/////////////////////////////////////////////////////////////////////
//
// Buffer implementation
// ======
Buffer::Buffer(bool shouldDelete):
  _eofRead{false},
  _shouldDelete{shouldDelete}
{
  _begin = _current = _end = _lexemeBegin = nullptr;
}

Buffer::~Buffer()
{
  if (_shouldDelete)
    delete []_begin;
}

String
Buffer::lexeme()
{
  String lexeme{_lexemeBegin, size_t(_current - _lexemeBegin)};

  _lexemeBegin = nullptr;
  return lexeme;
}

} // end namespace cg::parser
