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
// OVERVIEW: AbstractParser.cpp
// ========
// Source file for generic LL(n) parser.
//
// Author: Paulo Pagliosa
// Last revision: 07/02/2022

#include "AbstractParser.h"

namespace cg::parser
{ // begin namespace cg::parser


/////////////////////////////////////////////////////////////////////
//
// AbstractParser implementation
// ==============
void
AbstractParser::setInput(Buffer& input)
{
  _input = &input;
  _filename = _input->name();
}

void
AbstractParser::execute()
{
  if (_input != nullptr)
  {
    // init compilation unit
    initCompilationUnit();
    // start parser
    _lineNumber = 1;
    _token = nextToken();
    start();
    // terminate compilation unit
    terminateCompilationUnit();
  }
}

KeywordTableEntry*
AbstractParser::searchKeyword(KeywordTableEntry* k, const String& name) const
{
  for (; k->name != nullptr; k++)
    if (name == k->name)
      return k;
  return nullptr;
}

KeywordTableEntry*
AbstractParser::findKeyword(const String&) const
{
  return nullptr;
}

String
AbstractParser::errorMessageFormat(const char* msg) const
{
  constexpr auto errMsg = "Error %s %d: %s\n";
  constexpr auto maxLen = 1024;
  char fmt[maxLen];

  snprintf(fmt, maxLen, errMsg, _filename.c_str(), _lineNumber, msg);
  return fmt;
}

void
AbstractParser::initCompilationUnit()
{
  // do nothing
}

void
AbstractParser::terminateCompilationUnit()
{
  // do nothing
}

} // end namespace cg::parser
