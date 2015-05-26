#include "renderer/renderer.h"
#include "renderer/deferred_renderer.h"
#include "renderer/forward_renderer.h"

renderer_flags enabled_renderers = 0;
render_queue_item **render_queue;
int render_queue_size = 0;

void init_renderers(renderer_flags flags) {
  if (flags & GRAFFIKS_RENDERER_DEFERRED) {
    _init_renderer_df();
    enabled_renderers |= GRAFFIKS_RENDERER_DEFERRED;
  }

  if (flags & GRAFFIKS_RENDERER_FORWARD) {
    _init_renderer_fw();
    enabled_renderers |= GRAFFIKS_RENDERER_FORWARD;
  }
}

void terminate_renderers(renderer_flags flags) {
  if (flags & GRAFFIKS_RENDERER_DEFERRED) {
    _terminate_renderer_df();
    enabled_renderers &= ~GRAFFIKS_RENDERER_DEFERRED;
  }

  if (flags & GRAFFIKS_RENDERER_FORWARD) {
    _terminate_renderer_fw();
    enabled_renderers &= ~GRAFFIKS_RENDERER_FORWARD;
  }
}

void _clear(renderer_flags flags) {
  if (flags & GRAFFIKS_RENDERER_DEFERRED) {
    _clear_df();
  }

  if (flags & GRAFFIKS_RENDERER_FORWARD) {
    _clear_fw();
  }
}
