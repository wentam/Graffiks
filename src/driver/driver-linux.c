#include <GL/glew.h>
#include <GL/glxew.h>
#include "driver/driver-linux.h"

Window root;
GLint att[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, GLX_SAMPLE_BUFFERS, 1,
               GLX_SAMPLES, 8, None};
XVisualInfo *vi;
Colormap cmap;
XSetWindowAttributes swa;
GLXContext glc;
int _use_vsync = 1;

void init_graffiks_xorg(int window_width, int window_height, char *window_title,
                        void (*init)(int *width, int *height),
                        void (*update)(float time_step), void (*finish)(void)) {

  display = XOpenDisplay(NULL);
  root = DefaultRootWindow(display);
  vi = glXChooseVisual(display, 0, att);

  cmap = XCreateColormap(display, root, vi->visual, AllocNone);

  swa.colormap = cmap;
  swa.event_mask = ExposureMask | KeyPressMask;

  win = XCreateWindow(display, root, 0, 0, window_width, window_height, 0, vi->depth,
                      InputOutput, vi->visual, CWColormap | CWEventMask, &swa);

  XMapWindow(display, win);

  XStoreName(display, win, window_title);

  glc = glXCreateContext(display, vi, NULL, GL_TRUE);

  glXMakeCurrent(display, win, glc);

  GLenum err = glewInit();
  if (GLEW_OK != err) {
    fprintf(stderr, "glew error: %s\n", glewGetErrorString(err));
  }

  if (GLX_EXT_swap_control && _use_vsync) {
    glXSwapIntervalEXT(display, win, 0);
  }

  graffiks_setup(init, update, finish);
  _set_size(window_width, window_height);

  _init_graffiks();

  while (1) {
    _draw_frame();
  }
}

void use_vsync(int vsync) { _use_vsync = vsync; }

// 0 for off. Does nothing after init_graffiks_xorg
void set_antialiasing_samples(int samples) { att[7] = samples; }
