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
// OVERVIEW: ReaderBase.h
// ========
// Class definition for generic reader.
//
// Author: Paulo Pagliosa
// Last revision: 10/02/2022

#ifndef __ReaderBase_h
#define __ReaderBase_h

#include "AbstractParser.h"
#include "Scope.h"
#include <set>

namespace cg::parser
{ // begin namespace cg::parser


/////////////////////////////////////////////////////////////////////
//
// Reader: generic reader base class
// ======
class Reader: public ErrorHandler
{
public:
  class Parser;

  ~Reader() override = default;

  void setInput(const String& filename);

  virtual void execute();

protected:
  enum
  {
    COULD_NOT_FIND_FILE,
    OUT_OF_MEMORY,
    UNABLE_TO_OPEN_INPUT_FILE,
    lastErrorCode
  };

  Reference<FileBuffer> _input;
  std::set<String> _includedFiles;
  fs::path _currentPath;

  virtual Parser* makeParser() = 0;

  virtual void init();
  virtual void terminate();

  void include(const String&);

private:
  // "global" scope to be used by the parser
  Scope _scope;

  void parse(FileBuffer&);

  const char* findErrorMessage(int) const override;
  Reference<FileBuffer> makeBuffer(const String&) const;

  friend Parser;

}; // Reader


/////////////////////////////////////////////////////////////////////
//
// Reader::Parser: generic reader parser class
// ==============
class Reader::Parser: public AbstractParser
{
public:
  ~Parser() override;

  auto& reader() const
  {
    return *_reader;
  }

protected:
  enum
  {
    _EOF,
    _NAME = 256,
    _INTEGER,
    _FLOAT,
    _VEC2,
    _VEC3,
    _VEC4,
    _RGB,
    _HSV,
    _STRING,
    _DEFINE,
    _LENGTH,
    _NORMALIZE,
    _DOT,
    _CROSS,
    _AXES,
    _INCLUDE,
    lastToken
  };

  enum
  {
    UNEXPECTED_CHAR,
    SYNTAX,
    NAME_EXPECTED,
    CHAR_EXPECTED,
    INDEX_OUT_OF_RANGE,
    UNDEFINED_NAME,
    BAD_CAST,
    ILLEGAL_OPERATION,
    CANNOT_BE_NULL,
    MULTIPLE_DECLARATION_FOR,
    UNEXPECTED_END_OF_FILE_IN_COMMENT_STARTED_ON_LINE,
    NO_STRING_ENDING,
    STRING_EXPECTED,
    EMPTY_FILENAME,
    UNEXPECTED_LEXEME,
    lastErrorCode
  };

  union Value
  {
    int integer;
    float real;
    void* object;

  };

  String _lexeme;
  Value _tokenValue;

  Parser(Reader& reader):
    _reader{&reader},
    _currentScope{&reader._scope}
  {
    // do nothing
  }

  auto& currentScope() const
  {
    return *_currentScope;
  }

  int nextToken() override;

  void advance()
  {
    _token = nextToken();
  }

  void include()
  {
    // _INCLUDE
    advance();
    _reader->include(matchString());
  }

  void beginBlock()
  {
    _currentScope = new Scope{*_currentScope};
  }

  void endBlock()
  {
    if (auto parent = _currentScope->parent())
    {
      delete _currentScope;
      _currentScope = parent;
    }
  }

  void define(const String&, const Expression&);
  Expression access(const String&) const;

  void match(int);
  void matchEndOfBlock();

  auto matchInteger()
  {
    return matchValue<int>();
  }

  auto matchFloat()
  {
    return matchValue<float>();
  }

  auto matchVec2()
  {
    return matchValue<vec2f>();
  }

  auto matchVec3()
  {
    return matchValue<vec3f>();
  }

  auto matchVec4()
  {
    return matchValue<vec4f>();
  }

  auto matchColor()
  {
    return matchValue<Color>();
  }

  int matchIndex(int, int);
  String matchName();
  String matchString();
  String matchFilename();
  String matchOptionalString();

  Expression expression();
  Expression term();
  Expression factor();

private:
  Reader* _reader;
  Scope* _currentScope;

  template <typename T> T matchValue();

  DECLARE_KEYWORD_TABLE(Reader::Parser);
  DECLARE_ERROR_MESSAGE_TABLE(Reader::Parser);

}; // Reader::Parser

template <typename T>
T
Reader::Parser::matchValue()
{
  T value;

  try
  {
    value = expression().operator T();
  }
  catch (const BadCast& e)
  {
    error(BAD_CAST, e.message());
  }
  return value;
}

} // end namespace cg

#endif // __ReaderBase_h
