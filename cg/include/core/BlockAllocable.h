//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2024 Paulo Pagliosa.                        |
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
// OVERVIEW: BlockAllocable.h
// ========
// Class definition for block allocable object.
//
// Author: Paulo Pagliosa
// Last revision: 19/04/2024

#ifndef __BlockAllocable_h
#define __BlockAllocable_h

#include "core/AllocableObject.h"
#include "core/BlockAllocator.h"

namespace cg
{ // begin namespace cg

inline constexpr unsigned DflBlockSize = 64;


/////////////////////////////////////////////////////////////////////
//
// BlockAllocable: block allocable object class
// ==============
template <typename T, unsigned size = DflBlockSize>
class BlockAllocable: public AllocableObject<T, BlockAllocator<T, size>>
{
public:
#ifdef _DEBUG
  const unsigned blockSize = size;
#endif // _DEBUG

}; // BlockAllocable

} // end namespace cg

#endif // __BlockAllocable_h
