#ifndef VECTOR3_H
#define VECTOR3_H
#pragma once

#include <cmath>

namespace vector3
{
  template <class T>
  class Vector3
  {
    public:
      T x, y, z;
      Vector3() :x(T(0)), y(T(0)), z(T(0)){}
      Vector3(T ix) :x(ix), y(ix), z(ix){}
      Vector3(T ix, T iy, T iz) : x(ix), y(iy), z(iz) {}
      Vector3(const Vector3<T> &v) { x = v.x; y = v.y; z = v.z; }

      void operator=(const Vector3<T> &v){ x = v.x; y = v.y; z = v.z; }

  	  T magnitude() const {
        return sqrt(x*x + y*y + z*z);
      }
  	  void normalize() {
        T mag = magnitude();
        if (mag)
          *this *= 1 / mag;
      }
      T dot(const Vector3<T> &v) const{
		    return (x * v.x + y * v.y + z * v.z);
	    }
      Vector3<T> cross(const Vector3<T> &v) const{
        return Vector3<T>(
          y * v.z - z * v.y,
          z * v.x - x * v.z,
          x * v.y - y * v.x
        );
      }
      void scale(T factor){
        x *= factor; y *= factor; z *= factor;
      }

      Vector3& operator*=(const T& n){
        *this = *this * n;
        return *this;
      }
      Vector3& operator+=(const Vector3<T> &v){
        *this = *this + v;
        return *this;
      }
      Vector3& operator-=(const Vector3<T> &v){
        *this = *this - v;
        return *this;
      }
  };

  template <class T>
  Vector3<T> operator+(const Vector3<T>& a, const Vector3<T>& b)
  {
    return Vector3<T>(
      a.x + b.x,
      a.y + b.y,
      a.z + b.z
    );
  }

  template <class T>
  Vector3<T> operator-(const Vector3<T>& a, const Vector3<T>& b)
  {
    return Vector3<T>(
      a.x - b.x,
      a.y - b.y,
      a.z - b.z
    );
  }

  template <class T>
  Vector3<T> operator*(const Vector3<T>& a, const T& b)
  {
    return Vector3<T>(
      a.x*b,
      a.y*b,
      a.z*b
    );
  }

  template <class T>
  inline T dot(const Vector3<T>& a, const Vector3<T>& b)
  {
    return (a.x * b.x + a.y * b.y + a.z * b.z);
  }

  template <class T>
  Vector3<T> cross(const Vector3<T>& a, const Vector3<T>& b)
  {
    return Vector3<T>(
      a.y * b.z - a.z * b.y,
      a.z * b.x - a.x * b.z,
      a.x * b.y - a.y * b.x
    );
  }
}

typedef vector3::Vector3<double> Vec3d;
typedef vector3::Vector3<float> Vec3f;

#endif
