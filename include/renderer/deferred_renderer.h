#ifndef DEFERRED_RENDERER_H
#define DEFERRED_RENDERER_H
#include "object/object.h"

void _init_renderer_df();
void _draw_object_df(object *o);
void _terminate_renderer_df();
void _clear_df();
void _light_pass_df();

#endif
