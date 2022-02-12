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
// OVERVIEW: ReaderBase.cpp
// ========
// Source file for generic reader base.
//
// Author: Paulo Pagliosa
// Last revision: 07/02/2022

#include "math/Matrix3x3.h"
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
toInteger(const String& s)
{
  return std::stoi(s);
}

inline auto
toFloat(const String& s)
{
  return std::stof(s);
}


/////////////////////////////////////////////////////////////////////
//
// Reader implementation
// ======
static const char* _errorMessages[]
{
  "Could not find file '%s'", // COULD_NOT_FIND_FILE
  "Out of memory", // OUT_OF_MEMORY
  "Unable to open input file '%s'", // UNABLE_TO_OPEN_INPUT_FILE
};

void
Reader::setInput(const String& filename)
{
  _input = makeBuffer(filename);
  _currentPath = _input->path().parent_path();
}

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
Reader::include(const String& filename)
{
  fs::path path{filename};

  if (!path.is_absolute())
    path = _currentPath / path;
  if (auto s = path.string(); _includedFiles.find(s) == _includedFiles.end())
    if (auto input = makeBuffer(s); input != nullptr)
      parse(*input);
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

Reference<FileBuffer>
Reader::makeBuffer(const String& filename) const
{
  auto path = fs::absolute(filename);

  if (!fs::exists(path))
    error(COULD_NOT_FIND_FILE, filename.c_str());
  else if (fs::is_regular_file(path))
    try
    {
      Reference<FileBuffer> buffer = new FileBuffer{path};

      if (buffer->file().is_open())
        return buffer;
    }
    catch (const std::exception&)
    {
      // do nothing
    }
  error(UNABLE_TO_OPEN_INPUT_FILE, filename.c_str());
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
  KEYWORD("length", _LENGTH, 0)
  KEYWORD("normalize", _NORMALIZE, 0)
  KEYWORD("dot", _DOT, 0)
  KEYWORD("cross", _CROSS, 0)
  KEYWORD("axes", _AXES, 0)
  KEYWORD("include", _INCLUDE, 0)
  END_KEYWORD_TABLE;

DEFINE_ERROR_MESSAGE_TABLE(Reader::Parser, AbstractParser)
  ERROR_MESSAGE(UNEXPECTED_CHAR,
    "Unexpected char '%c'")
  ERROR_MESSAGE(SYNTAX,
    "Syntax")
  ERROR_MESSAGE(NAME_EXPECTED,
    "Name expected")
  ERROR_MESSAGE(CHAR_EXPECTED,
    "'%c' expected")
  ERROR_MESSAGE(INDEX_OUT_OF_RANGE,
    "Index %d out of range [%d,%d]")
  ERROR_MESSAGE(UNDEFINED_NAME,
    "Name '%s' is undefined")
  ERROR_MESSAGE(BAD_CAST,
    "Bad cast: %s")
  ERROR_MESSAGE(ILLEGAL_OPERATION,
    "Illegal operation: %s")
  ERROR_MESSAGE(MULTIPLE_DECLARATION_FOR,
    "Multiple declaration for '%s'")
  ERROR_MESSAGE(UNEXPECTED_END_OF_FILE_IN_COMMENT_STARTED_ON_LINE,
    "Unexpected end of file in comment started on line %d")
  ERROR_MESSAGE(NO_STRING_ENDING,
    "No string ending")
  ERROR_MESSAGE(STRING_EXPECTED,
    "String expected")
  ERROR_MESSAGE(EMPTY_FILENAME,
    "Empty filename")
  ERROR_MESSAGE(UNEXPECTED_LEXEME,
    "Unexpected '%s'")
END_ERROR_MESSAGE_TABLE;

Reader::Parser::~Parser()
{
  while (auto parent = _currentScope->parent())
  {
    delete _currentScope;
    _currentScope = parent;
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
    _lexeme = buffer().lexeme();
    if (auto k = findKeyword(_lexeme); k == nullptr)
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
 _next_char:
    if (auto c = *buffer(); c != '"')
    {
      if (c == 0 || c == '\n')
        error(NO_STRING_ENDING);
      ++buffer();
      goto _next_char;
    }
    _lexeme = buffer().lexeme();
    ++buffer();
    return _STRING;
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
Reader::Parser::define(const String& name, const Expression& e)
{
  if (_currentScope->contains(name))
    error(MULTIPLE_DECLARATION_FOR, name.c_str());
  else
    _currentScope->insert(name, e);
}

Expression
Reader::Parser::access(const String& name) const
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
  else if (_token < 256 && _token != _EOF)
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
      error(UNEXPECTED_LEXEME, _lexeme.c_str());
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

String
Reader::Parser::matchName()
{
  if (_token != _NAME)
    error(NAME_EXPECTED);

  auto name = _lexeme;

  advance();
  return name;

}

String
Reader::Parser::matchString()
{
  if (_token != _STRING)
    error(STRING_EXPECTED);

  auto string = _lexeme;

  advance();
  return string;
}

String
Reader::Parser::matchFilename()
{
  auto filename = matchString();

  if (filename.empty())
    error(EMPTY_FILENAME);
  return filename;
}

String
Reader::Parser::matchOptionalString()
{
  String string;

  if (_token == _STRING)
  {
    string = _lexeme;
    advance();
  }
  return string;
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

  switch (_token)
  {
    case '(':
      advance();
      e = expression();
      match(')');
      break;
    case '-':
      advance();
      e = -expression();
      break;
    case '+':
      advance();
      e = +expression();
      break;
    case _NAME:
      e = access(_lexeme);
      advance();
      break;
    case _INTEGER:
      e = _tokenValue.integer;
      advance();
      break;
    case _FLOAT:
      e = _tokenValue.real;
      advance();
      break;
    case '<':
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
      break;
    }
    case _VEC2:
    {
      vec2f v;

      advance();
      match('(');
      v.x = matchFloat();
      if (_token == ')')
      {
        advance();
        v.y = v.x;
      }
      else
      {
        match(',');
        v.y = matchFloat();
        match(')');
      }
      e = v;
      break;
    }
    case _VEC3:
    {
      vec3f v;

      advance();
      match('(');
      v.x = matchFloat();
      if (_token == ')')
      {
        advance();
        v.z = v.y = v.x;
      }
      else
      {
        match(',');
        v.y = matchFloat();
        match(',');
        v.z = matchFloat();
        match(')');
      }
      e = v;
      break;
    }
    case _VEC4:
    {
      vec4f v;

      advance();
      match('(');
      v.x = matchFloat();
      if (_token == ')')
      {
        advance();
        v.w = v.z = v.y = v.x;
      }
      else
      {
        match(',');
        v.y = matchFloat();
        match(',');
        v.z = matchFloat();
        match(',');
        v.w = matchFloat();
        match(')');
      }
      e = v;
      break;
    }
    case _RGB:
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
      break;
    }
    case _LENGTH:
      advance();
      match('(');
      e = matchVec3().length();
      match(')');
      break;
    case _NORMALIZE:
      advance();
      match('(');
      e = matchVec3().versor();
      match(')');
      break;
    case _DOT:
    {
      advance();
      match('(');

      auto v = matchVec3();

      match(',');
      e = v.dot(matchVec3());
      match(')');
      break;
    }
    case _CROSS:
    {
      advance();
      match('(');

      auto v = matchVec3();

      match(',');
      e = v.cross(matchVec3());
      match(')');
      break;
    }
    case _AXES:
    {
      advance();
      match('(');

      mat3f m;

      m[2] = matchVec3().versor();
      match(',');
      m[1] = matchVec3();
      match(')');
      if ((m[0] = m[1].cross(m[2]).versor()).isNull())
        error(CANNOT_BE_NULL, "right");
      m[1] = m[2].cross(m[0]);
      e = quatf{m}.eulerAngles();
      break;
    }
    default:
      error(SYNTAX);
  }
  return e;
}

} // end namespace cg::parser
