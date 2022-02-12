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
// OVERVIEW: Scope.h
// ========
// Class definition for simple scope.
//
// Author: Paulo Pagliosa
// Last revision: 03/02/2022

#ifndef __Scope_h
#define __Scope_h

#include "Expression.h"
#include <map>

namespace cg::parser
{ // begin namespace cg::parser


/////////////////////////////////////////////////////////////////////
//
// Scope: simple scope class
// =====
class Scope
{
public:
  Scope() = default;

  Scope(Scope& parent):
    _parent{&parent}
  {
    // do nothing
  }

  auto parent() const
  {
    return _parent;
  }

  bool lookup(const std::string&, Expression&) const;

  auto contains(const std::string& name) const
  {
    return _symbols.find(name) != _symbols.end();
  }

  void insert(const std::string& name, const Expression& e)
  {
    _symbols.emplace(name, e);
  }

  void clear()
  {
    _symbols.clear();
  }

private:
  using SymbolTable = std::map<std::string, Expression>;

  SymbolTable _symbols;
  Scope* _parent{};

}; // Scope

} // end namespace cg::parser

#endif // __Scope_h
