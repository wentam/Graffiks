#ifndef GFKS_INTERNAL_H
#define GFKS_INTERNAL_H

#include "graffiks/graffiks.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_xlib.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// Error handling
void gfks_err(gfks_error err, int debug_level, char *msg);

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

// ------------------
// ---gfks_device---
// ------------------

struct gfks_device_protected_struct {
  VkPhysicalDevice *vk_physical_device;
  VkPhysicalDeviceProperties *vk_physical_device_properties;
  VkPhysicalDeviceFeatures *vk_physical_device_features;
  VkDevice *vk_logical_device;
  unsigned int vk_queue_family_property_count;
  VkQueueFamilyProperties *vk_queue_family_properties; // array
};

#endif
