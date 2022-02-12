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
// OVERVIEW: Buffer.h
// ========
// Class definition for generic input buffer.
//
// Author: Paulo Pagliosa
// Last revision: 10/02/2022

#ifndef __Buffer_h
#define __Buffer_h

#include "core/SharedObject.h"
#include "StringRef.h"

namespace cg::parser
{ // begin namespace cg::parser


/////////////////////////////////////////////////////////////////////
//
// Buffer: generic input buffer class
// ======
class Buffer: public SharedObject
{
public:
  ~Buffer() override;

  Buffer(bool shouldDelete = true);

  virtual String name() const = 0;

  String lexeme();

  void beginLexeme()
  {
    _lexemeBegin = _current;
  }

  char operator *() const
  {
    return *_current;
  }

  char operator ++()
  {
    return advance();
  }

  char operator ++(int)
  {
    auto temp = *_current;

    advance();
    return temp;
  }

protected:
  bool _shouldDelete;
  bool _eofRead;
  char* _begin;
  char* _end;
  char* _current;
  char* _lexemeBegin;

  virtual char advance() = 0;

}; // Buffer

} // end namespace cg::parser

#endif // __Buffer_h
