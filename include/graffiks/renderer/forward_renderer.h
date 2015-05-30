#ifndef FORWARD_RENDERER_H
#define FORWARD_RENDERER_H
#include "graffiks/object/object.h"

void _init_renderer_fw();
void _draw_from_queue_fw();
void _draw_object_fw(object *o);
void _terminate_renderer_fw();
void _clear_fw();

#endif
