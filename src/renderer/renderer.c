#include "renderer/renderer.h"
#include "renderer/deferred_renderer.h"

void init_renderers(renderer_flags flags) {
  if (flags & GRAFFIKS_RENDERER_DEFERRED) {
    _init_renderer_df();
  }

  if (flags & GRAFFIKS_RENDERER_FORWARD) {
    _init_renderer_df();
  }
}

void destroy_renderers(renderer_flags flags) {
  if (flags & GRAFFIKS_RENDERER_DEFERRED) {
    _destroy_renderer_df();
  }

  if (flags & GRAFFIKS_RENDERER_FORWARD) {
    _destroy_renderer_fw();
  }
}

void draw_object(renderer_flags flags, object *o) {
  if (flags & GRAFFIKS_RENDERER_DEFERRED) {
    _draw_object_df(o);
  }

  if (flags & GRAFFIKS_RENDERER_FORWARD) {
    _draw_object_fw(o);
  }
}
