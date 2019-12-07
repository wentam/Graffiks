#include "graffiks/graffiks.h"
#include <vulkan/vulkan.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef GFKS_INTERNAL_H
#define GFKS_INTERNAL_H


typedef void (*free_func)(gfks_context *context);

struct gfks_context_protected_struct {
  VkInstance *vk_instance;
  int enabled_vulkan_extension_count;
  const char **enabled_extensions; 
};

struct gfks_surface_protected_struct {
  gfks_context *context;
};
#endif
