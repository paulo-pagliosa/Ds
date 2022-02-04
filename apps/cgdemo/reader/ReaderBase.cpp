//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2007, 2022 Orthrus Group.                         |
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
// OVERVIEW: ReaderBase.cpp
// ========
// Source file for generic reader base.
//
// Author: Paulo Pagliosa
// Last revision: 03/02/2022

#include "ReaderBase.h"
#include <cassert>
#include <cctype>
#include <cstdlib>

namespace cg::parser
{ // begin namespace cg::parser

//
// Auxiliary functions
//
inline auto
toInteger(const StringRef& s)
{
  return atoi(s.begin);
}

inline auto
toFloat(const StringRef& s)
{
  return (float)atof(s.begin);
}


/////////////////////////////////////////////////////////////////////
//
// Reader implementation
// ======
static const char* _errorMessages[]
{
  "Could not find file \'%s\'", // COULD_NOT_FIND_FILE
  "Out of memory", // OUT_OF_MEMORY
  "Unable to open input file \'%s\'", // UNABLE_TO_OPEN_INPUT_FILE
};

void
Reader::execute()
{
  if (_input != nullptr)
  {
    // init the reader
    init();
    // parse the input file
    parse(*_input);
    // terminate the reader
    terminate();
    // clear the set of included files
    _includedFiles.clear();
  }
}

void
Reader::init()
{
  // do nothing
}

void
Reader::terminate()
{
  // do nothing
}

void
Reader::include(const std::string& filename)
{
  if (_includedFiles.find(filename) != _includedFiles.end())
  {
    Reference<FileBuffer> input{makeBuffer(filename)};

    assert(input != nullptr);
    parse(*input);
  }
}

void
Reader::parse(FileBuffer& input)
{
  // make a parser to the input file
  if (Reference<Parser> parser{makeParser()}; parser != nullptr)
  {
    // add the input file to the set of included files
    _includedFiles.emplace(input.name());
    // set the parser input
    parser->setInput(input);
    // run the parser
    parser->execute();
  }
}

const char*
Reader::findErrorMessage(int code) const
{
  assert(code >= 0 && code < lastErrorCode);
  return _errorMessages[code];
}

FileBuffer*
Reader::makeBuffer(const std::string& filename) const
{
  auto path = fs::absolute(filename);

  if (!fs::exists(path))
    error(COULD_NOT_FIND_FILE, filename);
  else if (fs::is_regular_file(path))
    if (auto buffer = new FileBuffer{path}; buffer->file().is_open())
      return buffer;
    else
      delete buffer;
  error(UNABLE_TO_OPEN_INPUT_FILE, filename);
  return nullptr;
}


/////////////////////////////////////////////////////////////////////
//
// Reader::Parser implementation
// ==============
DEFINE_KEYWORD_TABLE(Reader::Parser, AbstractParser)
  KEYWORD("vec2", _VEC2, 0)
  KEYWORD("vec3", _VEC3, 0)
  KEYWORD("vec4", _VEC4, 0)
  KEYWORD("rgb", _RGB, 0)
  KEYWORD("hsv", _HSV, 0)
  KEYWORD("define", _DEFINE, 0)
  KEYWORD("include", _INCLUDE, 0)
END_KEYWORD_TABLE;

DEFINE_ERROR_MESSAGE_TABLE(Reader::Parser, AbstractParser)
  ERROR_MESSAGE(UNEXPECTED_CHAR,
    "Unexpected char \'%c\'")
  ERROR_MESSAGE(SYNTAX,
    "Syntax")
  ERROR_MESSAGE(NAME_EXPECTED,
    "Name expected")
  ERROR_MESSAGE(CHAR_EXPECTED,
    "\'%c\' expected")
  ERROR_MESSAGE(INDEX_OUT_OF_RANGE,
    "Index %d out of range [%d,%d]")
  ERROR_MESSAGE(UNDEFINED_NAME,
    "Name \'%s\' is undefined")
  ERROR_MESSAGE(BAD_CAST,
    "Bad cast: %s")
  ERROR_MESSAGE(ILLEGAL_OPERATION,
    "Illegal operation: %s")
  ERROR_MESSAGE(MULTIPLE_DECLARATION_FOR,
    "Multiple declaration for \'%s\'")
  ERROR_MESSAGE(UNEXPECTED_END_OF_FILE_IN_COMMENT_STARTED_ON_LINE,
    "Unexpected end of file in comment started on line %d")
  ERROR_MESSAGE(NO_FILE_NAME_ENDING,
    "No file name ending")
  ERROR_MESSAGE(BAD_FILE_NAME,
    "Bad file name")
  ERROR_MESSAGE(FILE_NAME_EXPECTED,
    "File name expected")
  ERROR_MESSAGE(UNEXPECTED_LEXEME,
    "Unexpected \'%s\'")
END_ERROR_MESSAGE_TABLE;

Reader::Parser::~Parser()
{
  while (auto scope = _currentScope->parent())
  {
    delete _currentScope;
    _currentScope = scope;
  }
}

int
Reader::Parser::nextToken()
{
  bool isFloat;

_whitespace:
  while (isspace(*buffer()))
    if (buffer()++ == '\n')
      ++_lineNumber;
  if (*buffer() == 0)
    return _EOF;
  if (*buffer() == '/')
  {
    if (++buffer() == '/')
    {
      while (*buffer() != '\n')
        if (buffer()++ == 0)
          return _EOF;
      ++buffer();
      ++_lineNumber;
      goto _whitespace;
    }
    else if (*buffer() == '*')
    {
      auto start = _lineNumber;

_comment:
      while (++buffer() != '*')
        if (*buffer() == 0)
          error(UNEXPECTED_END_OF_FILE_IN_COMMENT_STARTED_ON_LINE, start);
        else if (*buffer() == '\n')
          ++_lineNumber;
      if (++buffer() != '/')
        goto _comment;
      ++buffer();
      goto _whitespace;
    }
    return '/';
  }
  buffer().beginLexeme();
  if (auto c = *buffer(); isalpha(c) || c == '_')
  {
    while (++buffer() == '_' || isalnum(*buffer()))
      ;
    _tokenValue.name = _lexeme = buffer().lexeme();
    if (auto k = findKeyword(_tokenValue.name); k == nullptr)
      return _NAME;
    else
    {
      _tokenValue.object = k->value;
      return k->token;
    }
  }
  if (isdigit(*buffer()))
  {
    isFloat = false;
    while (isdigit(++buffer()))
      ;
    if (*buffer() == '.')
    {
_float:
      isFloat = true;
      while (isdigit(++buffer()))
        ;
    }
    if (toupper(*buffer()) == 'E')
    {
      isFloat = true;
      if (++buffer() == '+' || *buffer() == '-')
        buffer()++;
      if (isdigit(*buffer()))
        while (isdigit(++buffer()))
          ;
      else
        error(UNEXPECTED_CHAR, *buffer());
    }
    _lexeme = buffer().lexeme();
    if (isFloat)
    {
      _tokenValue.real = toFloat(_lexeme);
      return _FLOAT;
    }
    else
    {
      _tokenValue.integer = toInteger(_lexeme);
      return _INTEGER;
    }
  }
  if (*buffer() == '.')
  {
    if (isdigit(++buffer()))
      goto _float;
    error(UNEXPECTED_CHAR, *buffer());
  }
  if (*buffer() == '"')
  {
    buffer()++;
    buffer().beginLexeme();
    if (auto c = buffer()++; c != '"')
      if (c == 0 || c == '\n')
        error(NO_FILE_NAME_ENDING);
    _tokenValue.filename = buffer().lexeme();
    return _FILE_NAME;
  }

  auto c = buffer()++;

  switch (c)
  {
    case '{':
    case '}':
    case '+':
    case '*':
    case '/':
    case '-':
    case '(':
    case ')':
    case '[':
    case ']':
    case '<':
    case '>':
    case ',':
    case ':':
    case '=':
      break;
    default:
      error(UNEXPECTED_CHAR, c);
  }
  _lexeme = buffer().lexeme();
  return c;
}

void
Reader::Parser::define(const std::string& name, const Expression& e)
{
  if (_currentScope->contains(name))
    error(MULTIPLE_DECLARATION_FOR, name.c_str());
  else
    _currentScope->insert(name, e);
}

Expression
Reader::Parser::access(const std::string& name) const
{
  Expression e;

  if (!_currentScope->lookup(name, e))
    error(UNDEFINED_NAME, name.c_str());
  return e;
}

void
Reader::Parser::match(int token)
{
  if (_token == token)
    advance();
  else if (_token < 256)
    error(UNEXPECTED_CHAR, _token);
  else
    error(CHAR_EXPECTED, token);
}

void
Reader::Parser::matchEndOfBlock()
{
  if (_token != '}')
    if (_token == _EOF)
      error(CHAR_EXPECTED, '}');
    else
      error(UNEXPECTED_LEXEME, _lexeme.toString().c_str());
  advance();
}

int
Reader::Parser::matchIndex(int min, int max)
{
  auto i = matchInteger();

  if (i < min || i > max)
    error(INDEX_OUT_OF_RANGE, i, min, max);
  return i;
}

std::string
Reader::Parser::matchName()
{
  if (_token != _NAME)
    error(NAME_EXPECTED);

  auto name = _tokenValue.filename.toString();

  advance();
  return name;

}
std::string
Reader::Parser::matchFilename()
{
  if (_token != _FILE_NAME)
    error(FILE_NAME_EXPECTED);

  auto filename = _tokenValue.filename.toString();

  if (filename.empty())
    error(BAD_FILE_NAME);
  advance();
  return filename;
}

std::string
Reader::Parser::matchOptionalName()
{
  std::string name;

  if (_token == _NAME)
    name = _tokenValue.filename.toString();
  advance();
  return name;
}

Expression
Reader::Parser::expression()
{
  Expression e;

  try
  {
    for (e = term();;)
      if (_token == '+')
      {
        advance();
        e += term();
      }
      else if (_token == '-')
      {
        advance();
        e -= term();
      }
      else
        break;
  }
  catch (const IllegalOperation& e)
  {
    error(ILLEGAL_OPERATION, e.message());
  }
  return e;
}

Expression
Reader::Parser::term()
{
  Expression e;

  for (e = factor();;)
    if (_token == '*')
    {
      advance();
      e *= factor();
    }
    else if (_token == '/')
    {
      advance();
      e /= factor();
    }
    else
      return e;
}

Expression
Reader::Parser::factor()
{
  Expression e;

  if (_token == '(')
  {
    advance();
    e = expression();
    match(')');
  }
  else if (_token == '-')
  {
    advance();
    e = -expression();
  }
  else if (_token == '+')
  {
    advance();
    e = +expression();
  }
  else if (_token == _NAME)
  {
    e = access(_tokenValue.name.toString());
    advance();
  }
  else if (_token == _INTEGER)
  {
    e = _tokenValue.integer;
    advance();
  }
  else if (_token == _FLOAT)
  {
    e = _tokenValue.real;
    advance();
  }
  else if (_token == '<')
  {
    vec3f v;

    advance();
    v.x = matchFloat();
    match(',');
    v.y = matchFloat();
    match(',');
    v.z = matchFloat();
    match('>');
    e = v;
  }
  else if (_token == _VEC2)
  {
    vec2f v;

    advance();
    match('(');
    v.x = matchFloat();
    match(',');
    v.y = matchFloat();
    match(')');
    e = v;
  }
  else if (_token == _VEC3)
  {
    vec3f v;

    advance();
    match('(');
    v.x = matchFloat();
    match(',');
    v.y = matchFloat();
    match(',');
    v.z = matchFloat();
    match(')');
    e = v;
  }
  else if (_token == _VEC4)
  {
    vec4f v;

    advance();
    match('(');
    v.x = matchFloat();
    match(',');
    v.y = matchFloat();
    match(',');
    v.z = matchFloat();
    match(',');
    v.w = matchFloat();
    match(')');
    e = v;
  }
  else if (_token == _RGB)
  {
    Color c;

    advance();
    match('(');
    c.r = math::clamp<float>(matchFloat(), 0, 1);
    match(',');
    c.g = math::clamp<float>(matchFloat(), 0, 1);
    match(',');
    c.b = math::clamp<float>(matchFloat(), 0, 1);
    match(')');
    e = c;
  }
  else
    error(SYNTAX);
  return e;
}

} // begin namespace cg::parser
