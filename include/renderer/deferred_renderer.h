#ifndef DEFERRED_RENDERER_H
#define DEFERRED_RENDERER_H
#include <GL/gl.h>
#include "object/object.h"
#include "lights.h"

void _init_renderer_df();
void _draw_object_df(object *o);
void _terminate_renderer_df();
void _clear_df();
void _ambient_pass_df();
void _light_pass_point_df(point_light *light);
void _light_pass_df();
void _geom_pass_df();

#endif
