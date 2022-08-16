#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <string>
#include <math.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "vector.hpp"
#include "ray.hpp"
#include "scene.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include "clock.hpp"
#define PI 3.14159265

int main() {
  int fps_limit = 60;
  int threads = 8;

  /* Get Terminal Size */
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  int window_width = w.ws_col;
  int window_height = w.ws_row-1; // -3 because of the FPS and Frame time that is shown on the top
  /*
  window_width = 80;
  window_height = 22;
  */

  /* Framebuffer */
  char pixels[window_width * window_height];
  
  /* Init Window */ 
  Window window(window_width, window_height);
  window.fill(pixels);

  /* Init Renderer */ 
  Renderer renderer(window_width, window_height, true);

  /* Creating Scene */
  Camera camera(Vec3f(0.,0.,0.), Vec3f(0.,1.,0.), Vec3f(0.,0.,1.), 75);
  Vec3f light(0.,0.,20.);
  Object *objects[] = {new Cube(Vec3f(0.,0.,0.), Vec3f(1.,0.,0.), Vec3f(0.,3.,0.), Vec3f(0.,0.,3.), true),
                       new Cube2(Vec3f(-5.-0.8,-0.8,-3.-0.8), Vec3f(-5.+0.8,0.8,-3.+0.8), false),
                       new Cube2(Vec3f(-5.-0.8,0.-0.8,3.-0.8), Vec3f(-5.+0.8,0.+0.8,3.+0.8), false),
                       new Sphere(Vec3f(3.,0.,0.), 1.2, false),
                       new Sphere(Vec3f(-5.,-4.,0.), 1.2, false),
                       new Sphere(Vec3f(-5.,4.,0.), 1.2, false),
                       new Sphere(Vec3f(-5.,0.,0.), 1.2, false)};
  Object_List scene(objects, 7);

  /* Main Loop */
  system("clear"); // to clear any unnecessary stuff that is still on the screen
  Clock clock(fps_limit);
  window.show_cursor(false);
  float cam_angle = 0.;
  uint64_t frame = 0;
  while (cam_angle <= 360*4.) {
    frame++;
    // render and display
    renderer.threaded_render(&scene, &camera, &light, pixels, threads);
    clock.calculate_rendertime();
    clock.show_stats(pixels, &window_width, &frame);
    window.display(pixels);
    clock.calculate_displaytime();

    // update camera and light
    light.x = cos((cam_angle*PI)/180.)*50;
    light.y = sin((cam_angle*PI)/180.)*100;
    camera.view_point.x = cos((cam_angle*PI)/180.)*12;
    camera.view_point.y = sin((cam_angle*PI)/180.)*15;
    camera.view_point.z = sin((cam_angle*PI)/180.)*-5;
    camera.view_direction = Vec3f(0.,0.,0.) - camera.view_point;
    camera.view_direction = camera.view_direction.normalize();
    cam_angle += 70 * clock.frametime;

    clock.calculate_frametime();
  }
  window.show_cursor(true);
}
