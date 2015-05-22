#ifndef RENDERER_H
#define RENDERER_H
#include "object/object.h"

typedef enum {
  GRAFFIKS_RENDERER_DEFERRED = 0x01,
  GRAFFIKS_RENDERER_FORWARD = 0x02
} renderer_flags;

void init_renderers(renderer_flags flags);
void draw_object(renderer_flags flags, object *o);
void terminate_renderers(renderer_flags flags);

#endif
