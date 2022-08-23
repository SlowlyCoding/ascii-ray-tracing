#pragma once
#include "vector.hpp"
#include "ray.hpp"

/*
  Definition of the Camera class
*/
class Camera {
public:
  // view_point = location of camera
  // view_direction = in which direction the camera is looking
  // view_up = view_direction turned 90°, used for calculations in Renderer::render()
  Vec3f view_point, view_direction, view_up;
  int FOV;
  Camera(Vec3f view_point, Vec3f view_direction, Vec3f view_up, int FOV) : 
    view_point(view_point), view_direction(view_direction.normalize()), view_up(view_up.normalize()), FOV(FOV) {};
};


/*
  Whenever a ray hits an object the information of that intesection is stored here 
*/
struct intersection_information {
  float t;
  Vec3f point; 
  Vec3f normal;
  bool reflective_surface;
};



/*
  Every object inherits from this class
*/
class Object {
public:
  virtual bool intersection(Ray *ray, intersection_information *ii) {return false;}
};



/*
  Definition of all the objects like spheres and triangles

  Each object only has one function which calculates the intersection between a ray and that object
*/
class Sphere : public Object {
public:
  Vec3f center;
  float radius;
  bool reflective;
  Sphere(Vec3f center, float radius, bool reflective) : center(center), radius(radius), reflective(reflective) {};
  bool intersection(Ray *ray, intersection_information *ii);
};

class Triangle : public Object {
public:
  Vec3f p1, p2, p3;
  bool reflective;
  Triangle(Vec3f p1, Vec3f p2, Vec3f p3, bool reflective) : p1(p1), p2(p2), p3(p3), reflective(reflective) {};
  Triangle() : p1(Vec3f()), p2(Vec3f()), p3(Vec3f()), reflective(false) {};
  bool intersection(Ray *ray, intersection_information *ii);
};

class Checkerboard : public Object {
public:
  Vec3f plane_normal;
  float d;
  bool reflective;
  Checkerboard(Vec3f plane_normal, float d) : plane_normal(plane_normal), d(d) {};
  bool intersection(Ray *ray, intersection_information *ii);
};

// Cube made out of 12 triangles
class Cube : public Object {
private:
  Vec3f cube_corners[8];
  Triangle triangles[12];
public:
  Vec3f center;
  Vec3f center_to_side1;
  Vec3f center_to_side2;
  Vec3f center_to_side3;
  bool reflective;
  Cube(Vec3f center, Vec3f center_to_side1, Vec3f center_to_side2, Vec3f center_to_side3, bool reflective);
  bool intersection(Ray *ray, intersection_information *ii);
};
Cube::Cube(Vec3f center, Vec3f center_to_side1, Vec3f center_to_side2, Vec3f center_to_side3, bool reflective) {
  center = center; 
  center_to_side1 = center_to_side1;
  center_to_side2 = center_to_side2;
  center_to_side3 = center_to_side3;
  reflective = reflective;

  cube_corners[0] = center + center_to_side1 + center_to_side2 + center_to_side3;
  cube_corners[1] = center + center_to_side1 + center_to_side2 - center_to_side3;
  cube_corners[2] = center + center_to_side1 - center_to_side2 - center_to_side3;
  cube_corners[3] = center + center_to_side1 - center_to_side2 + center_to_side3;
  cube_corners[4] = center - center_to_side1 + center_to_side2 + center_to_side3;
  cube_corners[5] = center - center_to_side1 + center_to_side2 - center_to_side3;
  cube_corners[6] = center - center_to_side1 - center_to_side2 - center_to_side3;
  cube_corners[7] = center - center_to_side1 - center_to_side2 + center_to_side3;
  triangles[0]  = Triangle(cube_corners[0], cube_corners[1], cube_corners[2], reflective);
  triangles[1]  = Triangle(cube_corners[2], cube_corners[3], cube_corners[0], reflective);
  triangles[2]  = Triangle(cube_corners[4], cube_corners[5], cube_corners[6], reflective);
  triangles[3]  = Triangle(cube_corners[6], cube_corners[7], cube_corners[4], reflective);
  triangles[4]  = Triangle(cube_corners[0], cube_corners[1], cube_corners[5], false);
  triangles[5]  = Triangle(cube_corners[5], cube_corners[4], cube_corners[0], false);
  triangles[6]  = Triangle(cube_corners[1], cube_corners[2], cube_corners[6], false);
  triangles[7]  = Triangle(cube_corners[6], cube_corners[5], cube_corners[1], false);
  triangles[8]  = Triangle(cube_corners[2], cube_corners[3], cube_corners[7], false);
  triangles[9]  = Triangle(cube_corners[7], cube_corners[6], cube_corners[2], false);
  triangles[10] = Triangle(cube_corners[0], cube_corners[3], cube_corners[7], false);
  triangles[11] = Triangle(cube_corners[7], cube_corners[4], cube_corners[0], false);
}

// actual cube (bounding box)
// intersection is calculated using slab method
class Cube2 : public Object {
public:
  Vec3f bound_min, bound_max;
  bool reflective;
  Cube2(Vec3f bound_min, Vec3f bound_max, bool reflective) : bound_min(bound_min), bound_max(bound_max), reflective(reflective) {};
  bool intersection(Ray *ray, intersection_information *ii);
};



/*
  Object_List stores an array with all the objects in a scene

  Advantage of this is that we can just call Object_List.intersection() later on in our program
  and it will check intersections between all objects in our scene itself and return the closest one
*/
class Object_List : public Object {
public:
  Object **objects;
  float n; // number of objects
  Object_List(Object **objects, float n) : objects(objects), n(n) {};
  bool intersection(Ray *ray, intersection_information *ii);
};



/*
  Defining how each object calculates an intersection with a given ray

  returns false if there is no intersection at all
*/
bool Sphere::intersection(Ray *ray, intersection_information *ii) {
  float a = dot(ray->direction, ray->direction);
  float b = 2.f * dot(ray->direction, ray->origin - center);
  float c = dot(ray->origin - center, ray->origin - center) - radius*radius;
  float discriminant = b*b - 4.*a*c;
  if (discriminant > 0) {
    float t1 = (-b + sqrt(discriminant)) / 2.*a; 
    float t2 = (-b - sqrt(discriminant)) / 2.*a; 
    float t = std::fmin(t1, t2);
    if (t > ray->min_t && t < ray->max_t) {
      ii->t = t;
      ii->point = ray->point(t);
      ii->normal = (ray->point(t) - center).normalize();
      ii->reflective_surface = reflective;
      return true;
    }
  }
  return false;
}

bool Triangle::intersection(Ray *ray, intersection_information *ii) {
  // create a plane using the 3 triangle points
  Vec3f plane_normal = cross(p2-p1, p3-p1);
  float d = dot(p1, plane_normal);
  // calculate intersection between ray and plane
  if (dot(ray->direction, plane_normal) == 0.) {return false;}
  float t = (d - dot(ray->origin,plane_normal)) / dot(ray->direction, plane_normal);
  if (t < ray->min_t || t > ray->max_t) {return false;}
  // check if that intersection point is inside the triangle
  Vec3f ii_point = ray->point(t);
  if (dot(cross(p2-p1, ii_point-p1), plane_normal) >= 0 &&
      dot(cross(p3-p2, ii_point-p2), plane_normal) >= 0 &&
      dot(cross(p1-p3, ii_point-p3), plane_normal) >= 0) {
    ii->t = t;
    ii->point = ii_point;
    ii->normal = plane_normal.normalize();
    ii->reflective_surface = reflective;
    return true;
  }
  return false;
}

bool Checkerboard::intersection(Ray *ray, intersection_information *ii) {
  if (dot(ray->direction, plane_normal) == 0.) {return false;}
  float t = (d - dot(ray->origin, plane_normal)) / dot(ray->direction, plane_normal);
  Vec3f hitpoint = ray->point(t)/8; // scaling the plane
  // for each balck sqaure its coordinates are either both even or both odd
  if ((int)hitpoint.x % 2 == 0 && (int)hitpoint.y % 2 == 0 ||
      (int)hitpoint.x % 2 != 0 && (int)hitpoint.y % 2 != 0) {
    return false;
  }
  if (t > ray->min_t && t < ray->max_t) {
    ii->t = t;
    ii->point = ray->point(t);
    ii->normal = plane_normal.normalize();
    ii->reflective_surface = false;
    return true;
  }
  return false;
}

// calculates the closest of the 12 triangles
bool Cube::intersection(Ray *ray, intersection_information *ii) {
  bool any_intersection = false;
  intersection_information temp_ii;
  float closest_t = ray->max_t;
  for (int i=0; i<12; i++) {
    if (triangles[i].intersection(ray, &temp_ii)) {
      if (temp_ii.t < closest_t) {
        closest_t = temp_ii.t;
        *ii = temp_ii;
      }
      any_intersection = true;
    }
  }
  return any_intersection;
}

// slab method
bool Cube2::intersection(Ray *ray, intersection_information *ii) {
  float t_min = ray->min_t; 
  float t_max = ray->max_t; 

  Vec3f x_plane_normal(1.,0.,0.);
  float inverse_x = 1. / dot(ray->direction, x_plane_normal);
  float t_x0 = (bound_min.x - dot(ray->origin,x_plane_normal)) * inverse_x;
  float t_x1 = (bound_max.x - dot(ray->origin,x_plane_normal)) * inverse_x;
  if (dot(ray->direction, x_plane_normal) != 0.) {
    t_min = std::fmax(t_min, std::fmin(t_x0, t_x1));
    t_max = std::fmin(t_max, std::fmax(t_x0, t_x1));
  }
  Vec3f y_plane_normal(0.,1.,0.);
  float inverse_y = 1. / dot(ray->direction, y_plane_normal);
  float t_y0 = (bound_min.y - dot(ray->origin,y_plane_normal)) * inverse_y;
  float t_y1 = (bound_max.y - dot(ray->origin,y_plane_normal)) * inverse_y;
  if (dot(ray->direction, x_plane_normal) != 0.) {
    t_min = std::fmax(t_min, std::fmin(t_y0, t_y1));
    t_max = std::fmin(t_max, std::fmax(t_y0, t_y1));
  }
  Vec3f z_plane_normal(0.,0.,1.);
  float inverse_z = 1. / dot(ray->direction, z_plane_normal);
  float t_z0 = (bound_min.z - dot(ray->origin,z_plane_normal)) * inverse_z;
  float t_z1 = (bound_max.z - dot(ray->origin,z_plane_normal)) * inverse_z;
  if (dot(ray->direction, x_plane_normal) != 0.) {
    t_min = std::fmax(t_min, std::fmin(t_z0, t_z1));
    t_max = std::fmin(t_max, std::fmax(t_z0, t_z1));
  }
  
  if (t_min < t_max) {
    ii->t = t_min;
    ii->point = ray->point(t_min);
    ii->reflective_surface = false;
    Vec3f cube_center = bound_min + (bound_max-bound_min)*0.5f;
    Vec3f cti = (ii->point - cube_center).normalize(); // center to intersection (cti)
    if (std::abs(cti.x) >= std::abs(cti.y)) {
      if (std::abs(cti.x) >= std::abs(cti.z)) {
        ii->normal = Vec3f(round(cti.x),0.,0.); 
        ii->reflective_surface = false;
      } else {
        ii->normal = Vec3f(0.,0.,round(cti.z)); 
      }
    } else {
      if (std::abs(cti.y) >= std::abs(cti.z)) {
        ii->normal = Vec3f(0.,round(cti.y),0.); 
      } else {
        ii->normal = Vec3f(0.,0.,round(cti.z)); 
      }
    }
    return true;
  }
  return false;
}


/*
  Goes through all objects and returns the closest intersection
*/
bool Object_List::intersection(Ray *ray, intersection_information *ii) {
  bool any_intersection = false;
  intersection_information temp_ii;
  float closest_t = ray->max_t;
  for (int i=0; i<n; i++) {
    if (objects[i]->intersection(ray, &temp_ii)) {
      if (temp_ii.t < closest_t) {
        closest_t = temp_ii.t;
        *ii = temp_ii;
      }
      any_intersection = true;
    }
  }
  return any_intersection;
}
