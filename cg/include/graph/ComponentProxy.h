//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2022, 2023 Paulo Pagliosa.                        |
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
// OVERVIEW: ComponentProxy.h
// ========
// Class definition for generic component proxy.
//
// Author: Paulo Pagliosa
// Last revision: 13/06/2023

#ifndef __ComponentProxy_h
#define __ComponentProxy_h

#include "graph/Component.h"

namespace cg::graph
{ // begin namespace cg::graph


/////////////////////////////////////////////////////////////////////
//
// ComponentProxy: generic component proxy class
// ==============
template <typename T>
class ComponentProxy: public Component
{
protected:
  Reference<T> _object;

  ComponentProxy(const char* const typeName, const T& object):
    Component{typeName},
    _object{&object}
  {
    // do nothing
  }

}; // ComponentProxy

} // end namepace cg::graph

#endif // __ComponentProxy_h
