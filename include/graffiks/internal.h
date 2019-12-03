#include "graffiks/graffiks.h"
#include <vulkan/vulkan.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef GFKS_INTERNAL_H
#define GFKS_INTERNAL_H

struct gfks_context_struct {
  VkInstance *vk_instance;
  int enabled_vulkan_extension_count;
  const char **enabled_extensions; 
};

#endif
