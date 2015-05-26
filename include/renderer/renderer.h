#ifndef RENDERER_H
#define RENDERER_H
#include "object/object.h"

typedef enum {
  GRAFFIKS_RENDERER_DEFERRED = 0x01,
  GRAFFIKS_RENDERER_FORWARD = 0x02
} renderer_flags;

renderer_flags enabled_renderers;

typedef struct {
  mesh *mesh;
  material *material;
  object *parent_object;
} render_queue_item;

extern render_queue_item **render_queue;
extern int render_queue_size;

void init_renderers(renderer_flags flags);
void draw_object(renderer_flags flags, object *o);
void terminate_renderers(renderer_flags flags);
void _clear(renderer_flags flags);

#endif
