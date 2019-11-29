#include <GL/glew.h>
#include <GL/glxew.h>
#include "graffiks/graffiks.h"
#include "graffiks/dt_loop.h"
#include "graffiks/renderer/renderer.h"
#include "graffiks/dt_callbacks.h"
#include <stdio.h>
#include <stdlib.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glu.h>

Window root;
GLint att[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, GLX_SAMPLE_BUFFERS, 1,
               GLX_SAMPLES, 8, None};
XVisualInfo *vi;
Colormap cmap;
XSetWindowAttributes swa;
GLXContext glc;
int _use_vsync = 1;

void gfks_init_with_window(Display *idisplay, Window window) {
  win = window;
  display = idisplay;
 
  vi = glXChooseVisual(display, 0, att);

  int window_x;
  int window_y;
  unsigned int window_width;
  unsigned int window_height;
  unsigned int border_width;
  unsigned int depth;
  XGetGeometry(display, window, &root, &window_x, &window_y, &window_width,
                      &window_height, &border_width, &depth);

  XMapWindow(display, window);

  glc = glXCreateContext(display, vi, NULL, GL_TRUE);

  glXMakeCurrent(display, window, glc);

  GLenum err = glewInit();
  if (GLEW_OK != err) {
    fprintf(stderr, "glew error: %s\n", glewGetErrorString(err));
  }

  if (GLX_EXT_swap_control && _use_vsync) {
    glXSwapIntervalEXT(display, window, 1);
  }

  gfks_set_renderer_size(window_width, window_height);
}

void gfks_init(int window_width, int window_height, char *window_title) {
  display = XOpenDisplay(NULL);
  root = DefaultRootWindow(display);
  vi = glXChooseVisual(display, 0, att);

  cmap = XCreateColormap(display, root, vi->visual, AllocNone);

  swa.colormap = cmap;
  swa.event_mask = ExposureMask | KeyPressMask;

  Window window = XCreateWindow(display, root, 0, 0, window_width, window_height, 0, vi->depth,
                     InputOutput, vi->visual, CWColormap | CWEventMask, &swa);

  XMapWindow(display, window);

  XStoreName(display, window, window_title);
 
  gfks_init_with_window(display, window); 
}

void gfks_init_with_window_dt(Display *display, Window window,
                  void (*init)(int *width, int *height), void (*update)(float time_step),
                  void (*finish)(void)) {
  gfks_init_with_window(display, window);

  gfks_set_dt_callbacks(init, update, finish);

  _gfks_dt_start_loop();
}

void gfks_init_dt(int window_width, int window_height, char *window_title,
                  void (*init)(int *width, int *height), void (*update)(float time_step),
                  void (*finish)(void)) {
  gfks_init(window_width, window_height, window_title);

  gfks_set_dt_callbacks(init, update, finish);

  _gfks_dt_start_loop();
}

void gfks_use_vsync(int vsync) { _use_vsync = vsync; }

// 0 for off. Does nothing after init_graffiks_xorg
void gfks_set_antialiasing_samples(int samples) { att[7] = samples; }
