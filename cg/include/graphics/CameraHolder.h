//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2023, 2026 Paulo Pagliosa.                        |
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
// OVERVIEW: CameraHolder.h
// ========
// Class definition for camera holder.
//
// Author: Paulo Pagliosa
// Last revision: 29/08/2026

#ifndef __CameraHolder_h
#define __CameraHolder_h

#include "graphics/Camera.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// CameraHolder: camera holder class
// ============
class CameraHolder
{
public:
  [[nodiscard]] Camera* camera() const
  {
    return _camera;
  }

  void setCamera(Camera& camera)
  {
    if (&camera != _camera.get())
      _camera = &camera;
  }

protected:
  CameraHolder(Camera* camera = nullptr):
    _camera{!camera ? new Camera{} : camera}
  {
    // do nothing
  }

private:
  Reference<Camera> _camera;

}; // CameraHolder


/////////////////////////////////////////////////////////////////////
//
// SharedCameraHolder: shared camera holder class
// ==================
class SharedCameraHolder: public CameraHolder, public SharedObject
{
protected:
  using CameraHolder::CameraHolder;

}; // SharedCameraObject

} // end namespace cg

#endif // __CameraHolder_h
