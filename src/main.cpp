#include <unistd.h>
#include <sys/ioctl.h>
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
  int window_height = w.ws_row-1;

  /* Framebuffer */
  char pixels[window_width * window_height];
  
  /* Init Window */ 
  Window window(window_width, window_height);
  window.fill(pixels);

  /* Init Renderer */ 
  Renderer renderer(window_width, window_height, true);

  /* Creating Scene */
  Camera camera(Vec3f(0.,0.,15.), Vec3f(0.,1.,0.), Vec3f(0.,0.,1.), 75);
  Vec3f light(10.,20.,20.);
  Object *objects[] = {new Triangle(Vec3f(-20.,-20.,0.), Vec3f(20.,-20.,0.), Vec3f(20.,20.,0.), false),
                       new Triangle(Vec3f(-20.,-20.,0.), Vec3f(20.,20.,0.), Vec3f(-20.,20.,0.), false),
                       new Triangle(Vec3f(-20.,20.,2.), Vec3f(20.,20.,2.), Vec3f(20.,20.,10.), true),
                       new Triangle(Vec3f(-20.,20.,2.), Vec3f(-20.,20.,10.), Vec3f(20.,20.,10.), true),
                       new Cube2(Vec3f(-3.,-3.,0.), Vec3f(3.,3.,6.), false),
                       new Sphere(Vec3f(-8.,15.,2.), 2., false),
                       new Sphere(Vec3f(13.,10.,2.), 2., false),
                       new Sphere(Vec3f(-10.,-14.,2.), 2., false)};
  Object_List scene(objects, 8);

  /* positioning camera */
  camera.view_point.x = cos((45*PI)/180.)*35;
  camera.view_point.y = sin((225*PI)/180.)*38;
  camera.view_direction = Vec3f(0.,0.,0.) - camera.view_point;
  camera.view_direction = camera.view_direction.normalize();

  /* Main Loop */
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

    light.x = cos((cam_angle*PI)/180.)*20;
    light.y = sin((cam_angle*PI)/180.)*20;
    
    cam_angle += 50 * clock.frametime;

    clock.calculate_frametime();
  }
  window.show_cursor(true);
}
