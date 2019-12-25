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
bool gfks_check_alloc_throw_err(void *ptr, char *file, int line);

// -------------------
// ---gfks_defaults---
// -------------------

/// \private
typedef struct gfks_defaults_struct gfks_defaults;

/// \private
struct gfks_defaults_struct {
  gfks_rasterization_settings *rasterization_settings;
  gfks_multisample_settings *multisample_settings;
  void (*free)(gfks_defaults *defaults);
};

gfks_defaults* gfks_create_defaults();

// ------------------
// ---gfks_context---
// ------------------

/// \private
struct gfks_context_protected_struct {
  bool instance_created;
  VkInstance *vk_instance;
  int enabled_vulkan_extension_count;
  const char **enabled_extensions; 
  gfks_defaults *defaults;
};

// ------------------
// ---gfks_surface---
// ------------------

/// \private
struct gfks_surface_protected_struct {
  VkSurfaceKHR vk_surface;

  gfks_window_system window_system;
  void *window_handle; // data type differs based on window_system

  // Window geometry
  int window_x;
  int window_y;
  uint32_t window_width;
  uint32_t window_height;

};

typedef struct {
  Display *display;
  Window window;
} window_handle_x11;

// ------------------
// ---gfks_device---
// ------------------
// TODO we need to define which presentation queues can present to which surfaces
// TODO we need to make some of our device information public so the user of the
// engine can make decisions about it

/// \private
struct gfks_device_protected_struct {
  VkPhysicalDevice vk_physical_device;
  int vk_physical_device_index;
  VkPhysicalDeviceProperties *vk_physical_device_properties;
  VkPhysicalDeviceFeatures *vk_physical_device_features;
  VkDevice vk_logical_device;

  // Queue information
  unsigned int vk_queue_family_property_count;
  VkQueueFamilyProperties *vk_queue_family_properties; // array  

  unsigned int queue_count;
  VkQueue *queues;

  unsigned int *queue_families_for_queues;

  unsigned int graphics_queue_count;
  unsigned int *graphics_queue_indices;

  unsigned int compute_queue_count;
  unsigned int *compute_queue_indices;

  unsigned int transfer_queue_count;
  unsigned int *transfer_queue_indices;

  unsigned int sparse_binding_queue_count;
  unsigned int *sparse_binding_queue_indices;

  unsigned int presentation_queue_count;
  unsigned int *presentation_queue_indices;
};

// -----------------
// ---gfks_shader---
// -----------------

/// \private
struct gfks_shader_protected_struct {
  VkShaderModule vk_shader_module;
  VkPipelineShaderStageCreateInfo vk_shader_stage_create_info;
};

// ----------------------
// ---gfks_subpass---
// ----------------------

/// \private
/*typedef struct {
  VkSurfaceCapabilitiesKHR surface_capabilities;
  VkSurfaceFormatKHR surface_format;
  VkPresentModeKHR surface_presentation_mode;

  bool swap_chain_defined;
  VkSwapchainKHR swap_chain;

  VkExtent2D swap_chain_extent;

  uint32_t swap_chain_image_count;
  VkImage *swap_chain_images;
  VkImageView *swap_chain_image_views;
} presentation_surface_data;*/

typedef struct {
  uint32_t shader_count;
  gfks_shader **shader_set; // array of pointers to shaders
  gfks_rasterization_settings *rsettings; // pointer to settings
  gfks_multisample_settings *msettings; // pointer to settings
} draw_step;

/// \private
struct gfks_subpass_protected_struct {
  uint32_t draw_step_count;
  draw_step **draw_steps; // array of pointers to our draw steps

  //uint8_t presentation_surface_count;

  // TODO these two could be the same array (put the surface in the struct)
  //gfks_surface **presentation_surfaces; // array of pointers to gfks_surfaces
  //presentation_surface_data **presentation_surface_data; // array of pointers to our data

  // Viewport information
  VkViewport viewport;
  VkRect2D scissor;
  VkPipelineViewportStateCreateInfo viewport_state_create_info;
};


// ----------------------
// ---gfks_render_pass---
// ----------------------

typedef struct {
  gfks_surface *surface;

  VkSurfaceCapabilitiesKHR surface_capabilities;
  VkSurfaceFormatKHR surface_format;
  VkPresentModeKHR surface_presentation_mode;

  bool swap_chain_defined;
  VkSwapchainKHR swap_chain;

  VkExtent2D swap_chain_extent;

  uint32_t swap_chain_image_count;
  VkImage *swap_chain_images;
  VkImageView *swap_chain_image_views;
} presentation_surface;

typedef struct {
  gfks_subpass *subpass;
  uint32_t dep_count;
  uint32_t deps[16]; // array of indeces to render passes we depend on TODO should not be static size
} planned_subpass;

/// \private
struct gfks_render_pass_protected_struct {
  uint32_t subpass_count;
  planned_subpass *planned_subpasses;
  VkCommandBuffer *command_buffers; 
  uint32_t *subpass_order; // A pre-calculated order that satisfies dependencies. Array of indices.

  bool presentation_surface_defined;
  presentation_surface presentation_surface;
};

// ---------------------------------
// ---gfks_rasterization_settings---
// ---------------------------------

/// \private
struct gfks_rasterization_settings_protected_struct {
  VkPipelineRasterizationStateCreateInfo settings;
};

// ---------------------------------
// ---gfks_multisample_settings---
// ---------------------------------


/// \private
struct gfks_multisample_settings_protected_struct {
  VkPipelineMultisampleStateCreateInfo settings;
};


#endif
