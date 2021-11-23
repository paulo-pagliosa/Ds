//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2020 Orthrus Group.                         |
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
// OVERVIEW: Camera.cpp
// ========
// Source file for camera.
//
// Author: Paulo Pagliosa
// Last revision: 28/01/2020

#include "graphics/Camera.h"
#include <algorithm>
#include <stdexcept>

namespace cg
{ // begin namespace cg

//
// Auxiliary functions
//
inline mat4f
lookAt(const vec3f& p, const vec3f& u, const vec3f& v, const vec3f& n)
{
  mat4f m;

  m[0].set(u[0], v[0], n[0]);
  m[1].set(u[1], v[1], n[1]);
  m[2].set(u[2], v[2], n[2]);
  m[3].set(-u.dot(p), -v.dot(p), -n.dot(p), 1.0f);
  return m;
}

inline mat4f
lookAt(const vec3f& p, const mat3f r)
{
  return lookAt(p, r[0], r[1], r[2]);
}

inline void
transform(vec3f& p, const mat4f& m)
{
  p = m.transform3x4(p);
}

inline void
transformDirection(vec3f& v, const mat4f& m)
{
  v = m.transformVector(v).versor();
}


/////////////////////////////////////////////////////////////////////
//
// Camera implementation
// ======
uint32_t Camera::_nextId;

inline const char*
Camera::defaultName()
{
  static char name[16];

  snprintf(name, sizeof name, "Camera %d", ++_nextId);
  return name;
}

inline void
Camera::error(const std::string& message)
{
  throw std::runtime_error(message);
}

Camera::Camera():
  NameableObject{defaultName()},
  _timestamp{0}
{
  setDefaultView();
}

inline void
Camera::updateFocalPoint()
{
  _focalPoint = _position - viewPlaneNormal() * _distance;
}

inline void
Camera::updateView(const mat3f& r)
{
  _matrix = lookAt(_position, r);
  _inverseMatrix.set(r, _position);
  _modified = true;
}

void
Camera::setRotation(const mat3f& r)
{
  _rotation.set(r);
  _eulerAngles = _rotation.eulerAngles();
  updateView(r);
  updateFocalPoint();
}

void
Camera::setPosition(const vec3f& value)
//[]---------------------------------------------------[]
//|  Set the camera's position                          |
//|                                                     |
//|  Setting the camera's position will not change      |
//|  neither the direction of projection nor the        |
//|  distance between the position and the focal point. |
//|  The focal point will be moved along the direction  |
//|  of projection.                                     |
//[]---------------------------------------------------[]
{
  if (_position != value)
  {
    _position = value;
    updateView();
    updateFocalPoint();
  }
}

void
Camera::setEulerAngles(const vec3f& value)
{
  if (value != _eulerAngles)
  {
    _eulerAngles = value;
    _rotation = quatf::eulerAngles(value);
    updateView();
    updateFocalPoint();
  }
}

void
Camera::setRotation(const quatf& value)
{
  if (value != _rotation)
  {
    _rotation = value;
    _eulerAngles = _rotation.eulerAngles();
    updateView();
    updateFocalPoint();
  }
}

void
Camera::setDirectionOfProjection(const vec3f& value)
//[]---------------------------------------------------[]
//|  Set the direction of projection                    |
//|                                                     |
//|  Setting the direction of projection will not       |
//|  change the distance between the position and the   |
//|  focal point. The focal point will be moved along   |
//|  the direction of projection.                       |
//[]---------------------------------------------------[]
{
  if (value.isNull())
    error("DOP cannot be null");

  const auto n = -value.versor();
  const auto v = viewUp();
  const auto u = v.cross(n);

  if (u.isNull())
    error("DOP cannot be parallel to VUP");
  if (n != viewPlaneNormal())
    setRotation({u, v, n});
}

void
Camera::setViewUp(const vec3f& value)
//[]---------------------------------------------------[]
//|  Set the camera's view up                           |
//[]---------------------------------------------------[]
{
  if (value.isNull())
    error("VUP cannot be null");

  const auto n = viewPlaneNormal();
  const auto v = value.versor();
  const auto u = v.cross(n);

  if (u.isNull())
    error("VUP cannot be parallel to DOP");
  if (v != viewUp())
    setRotation({u, v, n});
}

void
Camera::setProjectionType(ProjectionType value)
//[]---------------------------------------------------[]
//|  Set the camera's projection type                   |
//[]---------------------------------------------------[]
{
  if (_projectionType != value)
  {
    _projectionType = value;
    updateProjection();
  }
}

void
Camera::setDistance(float value)
//[]---------------------------------------------------[]
//|  Set the camera's distance                          |
//|                                                     |
//|  Setting the distance between the position and      |
//|  focal point will move the focal point along the    |
//|  direction of projection.                           |
//[]---------------------------------------------------[]
{
  if (value <= 0)
    error("Distance must be positive");
  if (!math::isEqual(_distance, value))
  {
    _distance = std::max(value, MIN_DISTANCE);
    updateFocalPoint();
  }
}

void
Camera::setViewAngle(float value)
//[]---------------------------------------------------[]
//|  Set the camera's view angle                        |
//[]---------------------------------------------------[]
{
  if (value <= 0)
    error("View angle must be positive");
  if (!math::isEqual(_viewAngle, value))
  {
    _viewAngle = std::min(std::max(value, MIN_ANGLE), MAX_ANGLE);
    if (_projectionType == Perspective)
      updateProjection();
  }
}

void
Camera::setHeight(float value)
//[]---------------------------------------------------[]
//|  Set the camera's view_height                       |
//[]---------------------------------------------------[]
{
  if (value <= 0)
    error("Height of the view window must be positive");
  if (!math::isEqual(_height, value))
  {
    _height = std::max(value, MIN_HEIGHT);
    if (_projectionType == Parallel)
      updateProjection();
  }
}

void
Camera::setAspectRatio(float value)
//[]---------------------------------------------------[]
//|  Set the camera's aspect ratio                      |
//[]---------------------------------------------------[]
{
  if (value <= 0)
    error("Aspect ratio must be positive");
  if (!math::isEqual(_aspectRatio, value))
  {
    _aspectRatio = std::max(value, MIN_ASPECT);
    updateProjection();
  }
}

void
Camera::setClippingPlanes(float F, float B)
//[]---------------------------------------------------[]
//|  Set the distance of the clippling planes           |
//[]---------------------------------------------------[]
{
  if (F <= 0 || B <= 0)
    error("Clipping plane distance must be positive");
  if (F > B)
    std::swap(F, B);
  if (F < MIN_FRONT_PLANE)
    F = MIN_FRONT_PLANE;
  if ((B - F) < MIN_DEPTH)
    B = F + MIN_DEPTH;
  if (!math::isEqual(_F, F) || !math::isEqual(_B, B))
  {
    _F = F;
    _B = B;
    updateProjection();
  }
}

void
Camera::setNearPlane(float F)
//[]---------------------------------------------------[]
//|  Set the distance of the near clipping plane        |
//[]---------------------------------------------------[]
{
  if (F > MIN_FRONT_PLANE && _B - F > MIN_DEPTH && !math::isEqual(_F, F))
  {
    _F = F;
    updateProjection();
  }
}

inline void
Camera::rotate(const quatf& rotation)
{
  _rotation = rotation * _rotation;
  _eulerAngles = _rotation.eulerAngles();
}

void
Camera::azimuth(float angle)
//[]---------------------------------------------------[]
//|  Azimuth                                            |
//|                                                     |
//|  Rotate the camera's position about the view up     |
//|  vector centered at the focal point.                |
//[]---------------------------------------------------[]
{
  if (!math::isZero(angle))
  {
    rotate(quatf{angle, vec3f{0, 1, 0}});

    auto r = mat3f{_rotation};

    _position = _focalPoint + vec3f{r[2]} * _distance;
    updateView(r);
  }
}

void
Camera::elevation(float angle)
//[]---------------------------------------------------[]
//|  Elevation                                          |
//|                                                     |
//|  Rotate the camera's position about the cross       |
//|  product of the view plane normal and the view up   |
//|  vector centered at the focal point.                |
//[]---------------------------------------------------[]
{
  if (!math::isZero(angle))
  {
    rotate(quatf{angle, vec3f{1, 0, 0}});

    auto r = mat3f{_rotation};

    _position = _focalPoint + vec3f{r[2]} *_distance;
    updateView(r);
  }
}

void
Camera::roll(float angle)
//[]---------------------------------------------------[]
//|  Roll                                               |
//|                                                     |
//|  Rotate the view up vector around the view plane    |
//|  normal.                                            |
//[]---------------------------------------------------[]
{
  if (!math::isZero(angle))
  {
    rotate(quatf{angle, vec3f{0, 0, 1}});
    updateView();
  }
}

void
Camera::yaw(float angle)
//[]---------------------------------------------------[]
//|  Yaw                                                |
//|                                                     |
//|  Rotate the focal point about the view up vector    |
//|  centered at the camera's position.                 |
//[]---------------------------------------------------[]
{
  if (!math::isZero(angle))
  {
    rotate(quatf{angle, vec3f{0, 1, 0}});
    updateView();
    updateFocalPoint();
  }
}

void
Camera::pitch(float angle)
//[]---------------------------------------------------[]
//|  Pitch                                              |
//|                                                     |
//|  Rotate the focal point about the cross product of  |
//|  the view up vector and the view plane normal       |
//|  centered at the camera's position.                 |
//[]---------------------------------------------------[]
{
  if (!math::isZero(angle))
  {
    rotate(quatf{angle, vec3f{1, 0, 0}});
    updateView();
    updateFocalPoint();
  }
}

void
Camera::rotateYX(float ay, float ax, bool orbit)
//[]---------------------------------------------------[]
//|  Rotate YX                                          |
//|                                                     |
//|  If orbit is true, then it is a composition of an   |
//|  azimuth of ay with an elevation of ax, in this     |
//|  order. Otherwise, it is a composition of a yaw of  |
//|  ay with a pitch of ax, in this order.              |
//[]---------------------------------------------------[]
{
  auto q = quatf{ay, vec3f{0, 1, 0}} * _rotation;
  auto u = q.rotate(vec3f{1, 0, 0});

  _rotation = quatf{ax, u} * q;
  _eulerAngles = _rotation.eulerAngles();

  auto r = mat3f{_rotation};

  if (orbit)
  {
    _position = _focalPoint + vec3f{r[2]} *_distance;
    updateView(r);
  }
  else
  {
    updateView(r);
    updateFocalPoint();
  }
  _modified = true;
}

void
Camera::zoom(float zoom)
//[]---------------------------------------------------[]
//|  Zoom                                               |
//|                                                     |
//|  Change the view angle (or height) of the camera so |
//|  that more or less of a scene occupies the view     |
//|  window. A value > 1 is a zoom-in. A value < 1 is   |
//|  zoom-out.                                          |
//[]---------------------------------------------------[]
{
  if (zoom > 0)
    if (_projectionType == Perspective)
      setViewAngle(_viewAngle / zoom);
    else
      setHeight(_height / zoom);
}

void
Camera::translate(float dx, float dy, float dz)
//[]---------------------------------------------------[]
//|  Translate the camera                               |
//[]---------------------------------------------------[]
{
  if (!math::isZero(dx))
    _position += vec3f{_inverseMatrix[0]} * dx;
  if (!math::isZero(dy))
    _position += vec3f{_inverseMatrix[1]} * dy;
  if (!math::isZero(dz))
    _position += vec3f{_inverseMatrix[2]} * dz;
  updateView();
  updateFocalPoint();
  _modified = true;
}

void
Camera::setDefaultView(float aspect)
//[]---------------------------------------------------[]
//|  Set default view                                   |
//[]---------------------------------------------------[]
{
  _position.set(0.0f, 0.0f, 10.0f);
  _eulerAngles.set(0.0f);
  _rotation = quatf::identity();
  _focalPoint.set(0.0f, 0.0f, 0.0f);
  _distance = 10.0f;
  _aspectRatio = aspect;
  _projectionType = Perspective;
  _viewAngle = 60.0f;
  _height = 10.0f;
  _F = 0.01f;
  _B = 1000.0f;
  updateView();
  updateProjection();
}

uint32_t
Camera::update()
//[]---------------------------------------------------[]
//|  Update camera                                      |
//[]---------------------------------------------------[]
{
  if (_modified)
  {
    ++_timestamp;
    _modified = false;
  }
  return _timestamp;
}

void
Camera::updateProjection()
{
  if (_projectionType == Parallel)
  {
    auto t = _height * 0.5f;
    auto r = t * _aspectRatio;

    _projectionMatrix = mat4f::ortho(-r, r, -t, t, _F, _B);
  }
  else
    _projectionMatrix = mat4f::perspective(_viewAngle, _aspectRatio, _F, _B);
  _modified = true;
}

void
Camera::updateView()
{
  updateView(mat3f{_rotation});
}

inline const char*
projectionName(const Camera* camera)
//[]---------------------------------------------------[]
//|  Projection name                                    |
//[]---------------------------------------------------[]
{
  static const char* projectionName[] = {"Perspective", "Ortographic"};
  return projectionName[camera->projectionType()];
}

void
Camera::print(FILE* f) const
//[]---------------------------------------------------[]
//|  Print camera                                       |
//[]---------------------------------------------------[]
{
  fprintf(f, "Camera name: \"%s\"\n", name());
  fprintf(f, "Projection type: %s\n", projectionName(this));
  _position.print("Position: ", f);
  (-viewPlaneNormal()).print("Direction of projection: ", f);
  fprintf(f, "Distance: %f\n", _distance);
  viewUp().print("View up: ", f);
  fprintf(f, "View angle/_height: %f/%f\n", _viewAngle, _height);
}

} // end namespace cg
