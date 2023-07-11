//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2010, 2023 Paulo Pagliosa.                        |
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
// OVERVIEW: Writer.cpp
// ========
// Source file for generic writer.
//
// Author: Paulo Pagliosa
// Last revision: 10/07/2023

#include "Writer.h"
#include <cstdarg>

namespace cg::util
{ // begin namespace cg::util


/////////////////////////////////////////////////////////////////////
//
// Writer implementation
// ======
void
Writer::writeTabs()
//[]----------------------------------------------------[]
//|  Write tabs                                          |
//[]----------------------------------------------------[]
{
  if (_level == 0)
    return;
  for (int i{}; i < _level; i++)
    _out << "  ";
}

void
Writer::write(const char* format, ...)
//[]----------------------------------------------------[]
//|  Write                                               |
//[]----------------------------------------------------[]
{
  constexpr auto maxLen = 1024;
  char buffer[maxLen];
  va_list args;

  va_start(args, format);
  _out.write(buffer, vsnprintf(buffer, maxLen, format, args));
}

} // end namespace cg::util
