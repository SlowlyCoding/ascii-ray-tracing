#include "scene.hpp"
#include "vector.hpp"
#include "ray.hpp"
#include <vector>
#include <thread>

class Renderer {
public:
  int window_width, window_height;
  bool shadows; // if shadows should be rendered
  Renderer(int window_width, int window_height, bool shadows) : 
    window_width(window_width), window_height(window_height), shadows(shadows) {};
  /*
    Traces a ray through the scene and and returns the "color" of that pixel
    In this ray tracer colors are displayed using characters
  */
  char trace_ray(Object_List *scene, Ray *ray, Vec3f *light);
  /*
    Renders the Scene by calcuating what character each pixel should display
    render_framepart() renders a specified part of the frame
    threaded_render() creates threads and calls render_framepart()
  */
  void render_framepart(Object_List *scene, Camera *camera, Vec3f *light, char *pixels,
                        Vec3f pixel0, Vec3f pixel_step_x, Vec3f pixel_step_y, int thread_amount, int part);
  void threaded_render(Object_List *scene, Camera *camera, Vec3f *light, char *pixels, int thread_amount);
};


char Renderer::trace_ray(Object_List *scene, Ray *ray, Vec3f *light) {
  char grayscale[] = " .:-=+*#%@";
  int grayscale_length = sizeof(grayscale)/sizeof(grayscale[0])-1;

  char pixel = ' '; // default background pixel
  intersection_information ii;
  if (scene->intersection(ray, &ii)) {
    if (ii.reflective_surface) {
      // if the object that we just hit is reflective we shoot a new ray from that intersection point
      Vec3f reflected_ray_direction = ray->direction - ii.normal * 2.f*dot(ray->direction,ii.normal);
      Ray reflected_ray(ii.point + reflected_ray_direction*0.11f, reflected_ray_direction);
      pixel = trace_ray(scene, &reflected_ray, light);
    } else {
      // if the object is not refelctive we do shading
      Vec3f l = (*light - ii.point).normalize();
      if (shadows) {
        // check if the ray going to the light hits anything
        Ray light_ray(ii.point+l*0.01f, l);
        if (scene->intersection(&light_ray, &ii)) {
          // if it does the pixel is in shade
          //pixel = grayscale[0];
          pixel = ' ';
          return pixel;
        } 
      } 
      // if the pixel is getting light calculate lambertain lighting model
      float diffuse = std::max(0.f, dot(ii.normal, l));
      pixel = grayscale[(int)(diffuse*(grayscale_length))];
    }
  }
  return pixel;
}

void Renderer::render_framepart(Object_List *scene, Camera *camera, Vec3f *light, char *pixels, 
                                Vec3f pixel0, Vec3f pixel_step_x, Vec3f pixel_step_y, int thread_amount, int part) {
  // assign each thread a part of the frame
  int y = window_height * (1./thread_amount) * part;
  int y_max = window_height * (1./thread_amount) * (part+1);
  // go through each pixel of that part and call trace_ray()
  for (y; y<y_max; y++) {
    for (int x=0; x<window_width; x++) {
      Vec3f pixel = pixel0 + pixel_step_x*x + pixel_step_y*y;
      Ray ray(camera->view_point, pixel.normalize());
      pixels[window_width*y+x] = trace_ray(scene, &ray, light);
    }
  }
}
void Renderer::threaded_render(Object_List *scene, Camera *camera, Vec3f *light, char *pixels, int thread_amount) {
  // calculating different camera vectors
  Vec3f half_screen_x = cross(camera->view_direction, camera->view_up); 
  Vec3f half_screen_y = cross(camera->view_direction, half_screen_x)*2.f;
  half_screen_x = half_screen_x * (float)tan((camera->FOV/2.)*3.141592/180.);
  half_screen_y = half_screen_y * (float)tan(((camera->FOV*((float)window_height/window_width))/2.)*3.141592/180.);
  Vec3f pixel0 = camera->view_direction - half_screen_x - half_screen_y;
  Vec3f pixel_step_x = half_screen_x / ((float)window_width/2);
  Vec3f pixel_step_y = half_screen_y / ((float)window_height/2);

  // each thread renders its own part of the frame
  std::vector<std::thread> threads;
  for (int i=0; i<thread_amount; i++) {
    threads.push_back(std::thread(&Renderer::render_framepart, this, scene, camera, light, pixels, 
                                  pixel0, pixel_step_x, pixel_step_y, thread_amount, i));
  }
  for (auto& t : threads) t.join();
}
