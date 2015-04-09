#include "driver-linux.h"

Display *display;
Window root;
GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, None };
XVisualInfo *vi;
Colormap cmap;
XSetWindowAttributes swa;
Window win;
GLXContext glc;

void init_graffiks_xorg(int window_width, int window_height, char *window_title,
        void (*init)(int *width, int *height),
        void (*update)(float time_step),
        void (*draw)(void),
        void (*finish)(void)) {

    display = XOpenDisplay(NULL);
    root = DefaultRootWindow(display);
    vi = glXChooseVisual(display, 0, att);

    cmap = XCreateColormap(display, root, vi->visual, AllocNone);

    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask;

    win = XCreateWindow(display, root, 0, 0, window_width, window_height, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);

    XMapWindow(display, win);

    XStoreName(display, win, window_title);

    glc = glXCreateContext(display, vi, NULL, GL_TRUE);

    glXMakeCurrent(display, win, glc);

    _set_size(window_width,window_height);
    _init_graffiks();

    while(1) {
//  _draw_frame();
    }

 //   _finish();
}

