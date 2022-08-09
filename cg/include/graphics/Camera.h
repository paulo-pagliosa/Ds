//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2022 Paulo Pagliosa.                        |
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
// OVERVIEW: Camera.h
// ========
// Class definition for camera.
//
// Author: Paulo Pagliosa
// Last revision: 08/08/2022

#ifndef __Camera_h
#define __Camera_h

#include "core/Flags.h"
#include "core/NameableObject.h"
#include "math/Matrix4x4.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Camera: camera class
// ======
class Camera: public NameableObject
{
public:
  static constexpr float minAngle = 1;
  static constexpr float maxAngle = 120;
  static constexpr float minHeight = 0.01f;
  static constexpr float minAspect = 0.1f;
  static constexpr float minDistance = 0.01f;
  static constexpr float minFrontPlane = 0.01f;
  static constexpr float minDepth = 0.01f;

  enum ProjectionType
  {
    Perspective,
    Parallel
  };

  Camera(float aspect = 1);

  Camera(const Camera&) = delete;
  Camera& operator =(const Camera&) = delete;

  const vec3f& position() const;
  const vec3f& eulerAngles() const;
  const quatf& rotation() const;
  vec3f directionOfProjection() const;
  const vec3f& viewPlaneNormal() const;
  const vec3f& viewUp() const;
  float viewAngle() const;
  float height() const;
  float aspectRatio() const;
  float clippingPlanes(float& F, float& B) const;
  float nearPlane() const;
  ProjectionType projectionType() const;
  const vec3f& focalPoint() const;
  float distance() const;

  void setPosition(const vec3f& value);
  void setEulerAngles(const vec3f& value);
  void setRotation(const quatf& value);
  void setTransform(const vec3f& p, const quatf& q);
  void setDirectionOfProjection(const vec3f& value);
  void setViewUp(const vec3f& value);
  void setViewAngle(float value);
  void setHeight(float value);
  void setAspectRatio(float value);
  void setClippingPlanes(float F, float B);
  void setNearPlane(float F);
  void setProjectionType(ProjectionType value);
  void setDistance(float value);

  void set(const Camera&);
  void setProjection(const Camera&);
  void setDefaultView(float aspect = 1);
  void changeProjectionType();
  uint32_t update();

  uint32_t timestamp() const;
  bool modified() const;

  void azimuth(float angle);
  void elevation(float angle);
  void rotateYX(float ay, float ax, bool orbit = false);
  void roll(float angle);
  void yaw(float angle);
  void pitch(float angle);
  void zoom(float zoom);
  void translate(float dx, float dy, float dz);
  void translate(const vec3f& d);
  void translateNearPlane(float d);

  float windowHeight() const;
  const mat4f& worldToCameraMatrix() const;
  const mat4f& cameraToWorldMatrix() const;
  const mat4f& projectionMatrix() const;
  vec3f worldToCamera(const vec3f& p) const;
  vec3f cameraToWorld(const vec3f& p) const;

  void print(FILE* out = stdout) const;

private:
  static uint32_t _nextId;

  vec3f _position;
  vec3f _eulerAngles;
  quatf _rotation;
  float _viewAngle;
  float _height;
  float _aspectRatio;
  float _F;
  float _B;
  ProjectionType _projectionType;
  vec3f _focalPoint;
  float _distance;
  mat4f _worldToCamera;
  mat4f _cameraToWorld;
  mat4f _projectionMatrix;
  uint32_t _timestamp;
  bool _modified;

  static const char* defaultName();
  static void error(const std::string&);

  void updateFocalPoint();
  void setRotation(const mat3f&);
  void rotate(float, const vec3f&);
  void updateView(const mat3f&);
  void updateView();
  void updateProjection();

  friend class Renderer;

}; // Camera

inline const vec3f&
Camera::position() const
{
  return _position;
}

inline const vec3f&
Camera::eulerAngles() const
{
  return _eulerAngles;
}

inline const quatf&
Camera::rotation() const
{
  return _rotation;
}

inline const vec3f&
Camera::viewUp() const
{
  return (const vec3f&)(_cameraToWorld[1]);
}

inline const vec3f&
Camera::viewPlaneNormal() const
{
  return (const vec3f&)(_cameraToWorld[2]);
}

inline vec3f
Camera::directionOfProjection() const
{
  return -viewPlaneNormal();
}

inline Camera::ProjectionType
Camera::projectionType() const
{
  return _projectionType;
}

inline const vec3f&
Camera::focalPoint() const
{
  return _focalPoint;
}

inline float
Camera::distance() const
{
  return _distance;
}

inline float
Camera::viewAngle() const
{
  return _viewAngle;
}

inline float
Camera::height() const
{
  return _height;
}

inline float
Camera::aspectRatio() const
{
  return _aspectRatio;
}

inline float
Camera::clippingPlanes(float& F, float& B) const
{
  F = _F;
  B = _B;
  return B - F;
}

inline float
Camera::nearPlane() const
{
  return _F;
}

inline float
Camera::windowHeight() const
{
  if (_projectionType == Parallel)
    return _height;
  return 2 * _F * tan(math::toRadians(_viewAngle) * 0.5f);
}

inline uint32_t
Camera::timestamp() const
{
  return _timestamp;
}

inline bool
Camera::modified() const
{
  return _modified != 0;
}

inline void
Camera::translate(const vec3f& d)
{
  if (d.isNull())
    return;
  translate(d.x, d.y, d.z);
}

inline void
Camera::translateNearPlane(float d)
{
  setNearPlane(_F + d);
}

inline void
Camera::changeProjectionType()
{
  setProjectionType(_projectionType == Parallel ? Perspective : Parallel);
}

inline const mat4f&
Camera::worldToCameraMatrix() const
{
  return _worldToCamera;
}

inline const mat4f&
Camera::cameraToWorldMatrix() const
{
  return _cameraToWorld;
}

inline const mat4f&
Camera::projectionMatrix() const
{
  return _projectionMatrix;
}

inline vec3f
Camera::worldToCamera(const vec3f& p) const
{
  return _worldToCamera.transform3x4(p);
}

inline vec3f
Camera::cameraToWorld(const vec3f& p) const
{
  return _cameraToWorld.transform3x4(p);
}

//
// Auxiliary function
//
inline auto
vpMatrix(const Camera* c)
{
  return c->projectionMatrix() * c->worldToCameraMatrix();
}

} // end namespace cg

#endif // __Camera_h
