#ifndef GFKS_INTERNAL_H
#define GFKS_INTERNAL_H

#include "graffiks/graffiks.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_xlib.h>

#include <stdlib.h>
#include <stdio.h>

// Error handling
gfks_err(gfks_error err, int debug_level, int line, char *msg);

// ------------------
// ---gfks_context---
// ------------------

struct gfks_context_protected_struct {
  VkInstance *vk_instance;
  int enabled_vulkan_extension_count;
  const char **enabled_extensions; 
};

// ------------------
// ---gfks_surface---
// ------------------

struct gfks_surface_protected_struct {
  VkSurfaceKHR *vk_surface;

  gfks_window_system window_system;
  void *window_handle; // data type differs based on window_system
};

typedef struct {
  Display *display;
  Window window;
} window_handle_x11;

#endif
