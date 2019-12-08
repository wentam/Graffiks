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
  VkSurfaceKHR vk_surface;

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
// TODO we need to define which presentation queues can present to which surfaces
struct gfks_device_protected_struct {
  VkPhysicalDevice vk_physical_device;
  int vk_physical_device_index;
  VkPhysicalDeviceProperties *vk_physical_device_properties;
  VkPhysicalDeviceFeatures *vk_physical_device_features;
  VkDevice vk_logical_device;

  // Queue information
  unsigned int vk_queue_family_property_count;
  VkQueueFamilyProperties *vk_queue_family_properties; // array  

  unsigned int graphics_queue_count;
  VkQueue *graphics_queues;

  unsigned int compute_queue_count;
  VkQueue *compute_queues;

  unsigned int transfer_queue_count;
  VkQueue *transfer_queues;

  unsigned int sparse_binding_queue_count;
  VkQueue *sparse_binding_queues;

  unsigned int presentation_queue_count;
  VkQueue *presentation_queues;
};

#endif
