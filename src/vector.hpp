#include <math.h>

class Vec3f {
public:
  float x,y,z;
  Vec3f() : x(0.), y(0.), z(0.) {};
  Vec3f(float x, float y, float z) : x(x), y(y), z(z) {};
  Vec3f friend operator + (Vec3f v1, Vec3f v2);
  Vec3f friend operator - (Vec3f v1, Vec3f v2);
  Vec3f friend operator * (Vec3f v, float s);
  Vec3f friend operator * (Vec3f v, int s);
  Vec3f friend operator / (Vec3f v, float s);
  Vec3f friend operator / (Vec3f v, int s);
  float length();
  Vec3f normalize();
  void values();
};
float dot(Vec3f v1, Vec3f v2);
Vec3f cross(Vec3f v1, Vec3f v2);

Vec3f operator + (Vec3f v1, Vec3f v2) {
  return Vec3f(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}
Vec3f operator - (Vec3f v1, Vec3f v2) {
  return Vec3f(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}
Vec3f operator * (Vec3f v, float s) {
  return Vec3f(v.x * s, v.y * s, v.z * s);
}
Vec3f operator * (Vec3f v, int s) {
  return Vec3f(v.x * s, v.y * s, v.z * s);
}
Vec3f operator / (Vec3f v, float s) {
  return Vec3f(v.x / s, v.y / s, v.z / s);
}
Vec3f operator / (Vec3f v, int s) {
  return Vec3f(v.x / s, v.y / s, v.z / s);
}

float Vec3f::length() {
  return sqrt(x*x + y*y + z*z);
}
Vec3f Vec3f::normalize() {
  float l = length();
  return Vec3f(x/l, y/l, z/l);
}
void Vec3f::values() {
  std::cout << "(" << x << "," << y << "," << z << ")" << std::endl;
}

float dot(Vec3f v1, Vec3f v2) {
  return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}
Vec3f cross(Vec3f v1, Vec3f v2) {
  return Vec3f(v1.y*v2.z - v1.z*v2.y,
               v1.z*v2.x - v1.x*v2.z,
               v1.x*v2.y - v1.y*v2.x);
}
