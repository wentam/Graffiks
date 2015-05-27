#include "graffiks.h"

void (*init_func)(int *width, int *height);
void (*update_func)(float time_step);
void (*draw_func)(void);
void (*finish_func)(void);

int draw_callback_set = 0;

void graffiks_setup(void (*init)(int *width, int *height),
                    void (*update)(float time_step), void (*finish)(void)) {
  init_func = init;
  update_func = update;
  finish_func = finish;
}

void set_draw_callback(void (*draw)(void)) {
  draw_callback_set = 1;
  draw_func = draw;
}

void _call_init(int *width, int *height) { init_func(width, height); }
void _call_update(float time_step) { update_func(time_step); }
void _call_draw() {
  if (draw_callback_set == 1) {
    draw_func();
  }
}
void _call_finish() { finish_func(); }
