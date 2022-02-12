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
// OVERVIEW: AbstractParser.h
// ========
// Class definition for generic LL(n) parser.
//
// Author: Paulo Pagliosa
// Last revision: 10/02/2022

#ifndef __AbstractParser_h
#define __AbstractParser_h

#include "FileBuffer.h"
#include "ErrorHandler.h"

namespace cg::parser
{ // begin namespace cg::parser


/////////////////////////////////////////////////////////////////////
//
// KeywordTableEntry: keyword table entry class
// =================
class KeywordTableEntry
{
public:
  const char* name;
  int token;
  void* value;

}; // KeywordTableEntry


/////////////////////////////////////////////////////////////////////
//
// AbstractParser: generic LL(n) parser
// ==============
class AbstractParser: public ErrorHandler
{
public:
  ~AbstractParser() override = default;

  void setInput(Buffer&);
  virtual void execute();

protected:
  int _lineNumber{};
  int _token{};

  auto& buffer()
  {
    return *_input;
  }

  virtual int nextToken() = 0;

  virtual KeywordTableEntry* findKeyword(const String&) const;
  KeywordTableEntry* searchKeyword(KeywordTableEntry*, const String&) const;

  String errorMessageFormat(const char*) const override;

  virtual void initCompilationUnit();
  virtual void terminateCompilationUnit();

private:
  Reference<Buffer> _input;
  String _filename;

  virtual void start() = 0;

}; // AbstractParser

//
// Macros to declare a keyword table
//
#define DECLARE_KEYWORD_TABLE(cls) \
private: \
  static KeywordTableEntry _keywords[]; \
protected: \
  KeywordTableEntry* findKeyword(const String&) const override

#define DEFINE_KEYWORD_TABLE_ENTRIES(cls) \
cg::parser::KeywordTableEntry cls::_keywords[]{

#define KEYWORD(name, token, value) \
{name, token, (void*)value},

#define END_KEYWORD_TABLE \
{nullptr, -1, nullptr}}

#define DEFINE_KEYWORD_TABLE(cls, base) \
cg::parser::KeywordTableEntry* \
cls::findKeyword(const cg::parser::String& name) const\
{ \
  auto kw = searchKeyword(_keywords, name); \
  if (nullptr == kw) \
    kw = base::findKeyword(name); \
  return kw; \
} \
DEFINE_KEYWORD_TABLE_ENTRIES(cls)

} // end namespace cg::parser

#endif // __AbstractParser_h
