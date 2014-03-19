#include "graffiks.h"

void (*init_func)(void);
void (*update_func)(float time_step);
void (*draw_func)(void);
void (*finish_func)(void);

void graffiks_setup(void (*init)(void), void (*update)(float time_step), void (*draw)(void), void (*finish)(void)) {
    init_func = init;
    update_func = update;
    draw_func = draw;
    finish_func = finish;
}

void _call_init() {
    init_func();
}

void _call_update(float time_step) {
    update_func(time_step);
}

void _call_draw() {
    draw_func();
}

void _call_finish() {
    finish_func();
}
