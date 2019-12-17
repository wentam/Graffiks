/// \file

#ifndef GFKS_DRIVER_H
#define GFKS_DRIVER_H

#include <stdbool.h>
#include <stdint.h>

#ifdef GFKS_CONFIG_X11_SUPPORT
#include <X11/Xlib.h>
#endif

#define GFKS_ENGINE_NAME "Graffiks"
#define GFKS_DEBUG_TAG "GFKS"
#define GFKS_MAJOR_VERSION 0
#define GFKS_MINOR_VERSION 0
#define GFKS_REVISION 0

#ifndef GFKS_DEBUG_LEVEL
#define GFKS_DEBUG_LEVEL 0
#endif


// Core public interface
// TODO document what errors each function can return

// Error handling
// if any gfks_ function returns NULL instead of an object, look at gfks_latest_error

typedef enum {
  GFKS_ERROR_NONE,
  GFKS_ERROR_NULL_ARGUMENT,
  GFKS_ERROR_FAILED_MEMORY_ALLOCATION,
  GFKS_ERROR_VULKAN_EXTENSION_NOT_AVAILABLE,
  GFKS_ERROR_UNKNOWN,
  GFKS_ERROR_UNKNOWN_VULKAN,
  GFKS_ERROR_NO_VULKAN_DEVICE,
  GFKS_ERROR_DEVICE_NOT_SUITABLE_FOR_SURFACE,
  GFKS_ERROR_NO_SUITABLE_SURFACE_FORMAT,
  GFKS_ERROR_INVALID_SPIRV_DATA
} gfks_error;

gfks_error gfks_latest_error;

// Data types
typedef enum {
  GFKS_WINDOW_SYSTEM_NONE_BITFLAG = 1,
  GFKS_WINDOW_SYSTEM_X11_BITFLAG = 2,
  GFKS_WINDOW_SYSTEM_WAYLAND_BITFLAG = 4
} gfks_window_system;


// Class definitions

typedef struct gfks_context_struct gfks_context;
typedef struct gfks_surface_struct gfks_surface;
typedef struct gfks_device_struct gfks_device;
typedef struct gfks_shader_struct gfks_shader;
typedef struct gfks_render_pass_struct gfks_render_pass;
typedef struct gfks_render_plan_struct gfks_render_plan;
typedef struct gfks_rasterization_settings_struct gfks_rasterization_settings;

// --------------------
// --- gfks_context ---
// --------------------

typedef struct gfks_context_protected_struct gfks_context_protected;

/// gfks_context
struct gfks_context_struct {
  /// \private
  gfks_context_protected *_protected;

  /// \public
  /// \brief Frees a Graffiks context
  ///
  /// Must be called when you're done!
  /// \param gfks_context A context to be destroyed
  /// \memberof gfks_context_struct
  void (*free)(gfks_context *context);
};

/// \brief Creates a new Graffiks context
///
/// The window systems you request are not garuanteed to be initialized. Inspect window_systems after calling to make sure the window systems you need have been initialized!
///
/// The most likely reason for a window system to not be initialized would be the lack of required vulkan extensions.
/// 
/// \param window_systems Bit flags for the windows systems you will be drawing to (X11/Wayland/windows DWM/etc). Will be modified to only include windows systems that were initialized and GFKS_WINDOW_SYSTEM_NONE_BITFLAG
/// \returns A Graffiks context. NULL if there was an error.
/// \memberof gfks_context_struct
gfks_context* gfks_create_context(gfks_window_system *window_systems);

// --------------------
// --- gfks_surface ---
// --------------------

// TODO: can we make our surface device-agnostic?
// right now, surface->set_draw_device is needed so we can create a swap chain for the surface.
//
// I see a couple of ways to rework this.
// * Automatically set ourselves up for every device that ends up being created within our context,
// storing swapchains in arrays.
//
// This isn't a performance issue in CPU-space really as it's all init-time stuff, though I wonder
// about wasteing gpu memory with unused swap chains.
//
// * we could set up swap chains for different surfaces up in the render pass. This would require
// that the render pass is "initialized" with the surface before the render pass would be capable of
// presenting to the surface.

typedef struct gfks_surface_protected_struct gfks_surface_protected;

/// gfks_surface
struct gfks_surface_struct {
  /// \private
  gfks_surface_protected *_protected;

  /// \public
  /// \brief Read only. The parent Graffiks context.
  /// \memberof gfks_surface_struct
  gfks_context *context;

  // TODO function for user to get surface size (use window size or swap chain extent size?)

  /// \public
  /// \brief Frees a surface
  ///
  /// Must be called when you're done!
  /// \param gfks_surface A surface to be destroyed
  /// \memberof gfks_surface_struct
  void (*free)(gfks_surface *surface);
};

/// \brief Creates a new Graffiks surface
///
/// CURRENTLY UNIMPLEMENTED
///
/// \returns A Graffiks context. NULL if there was an error.
/// \memberof gfks_surface_struct
gfks_surface* gfks_create_surface();

/// \brief Creates a new Graffiks surface for an X11 window
///
/// Only present when GFKS_CONFIG_X11_SUPPORT is defined at build time.
///
/// \param display An X11 display
/// \param window An X11 window
/// \returns A Graffiks context. NULL if there was an error.
/// \memberof gfks_surface_struct
gfks_surface* gfks_create_surface_X11(gfks_context *context, Display *display, Window window);


// --------------------
// --- gfks_device ---
// --------------------

typedef struct gfks_device_protected_struct gfks_device_protected;

/// gfks_device
struct gfks_device_struct {
  /// \private
  gfks_device_protected *_protected;

  /// \public
  /// \brief The parent Graffiks context.
  /// \memberof gfks_device_struct
  gfks_context *context;

  /// \public
  /// \brief Sets up this device to draw to the specified surfaces.
  ///
  /// If you used gfks_get_devices_suitable_for_surfaces(), this has been done for you already.
  ///
  /// It's not guaranteed that any device can draw to any surface,
  /// such as the device not being connected to the correct display,
  /// so inspect that your device is capable of drawing to your surface with
  /// method suitable_for_surface.
  ///
  /// \param device The device we are setting up
  /// \param surfaces The surfaces we want to set this device up to draw to
  /// \param surface_count The number of surfaces we passed to the surfaces parameter
  /// \returns true on success, false on error
  /// \memberof gfks_device_struct
  bool (*set_up_for_surfaces)(gfks_device *device, gfks_surface *surfaces, uint32_t surface_count);

  /// \public
  /// \brief Determines if this device is able to, and has been set up to draw to the specified surface.
  ///
  /// The device can only be suitable for the surface if it has been set up with the set_up_for_surfaces() method
  ///
  /// \param device The device we want to test suitability for
  /// \param surface The surface we would like to test suitability against
  /// \returns true if this device is suitable for drawing to this surface, otherwise false.
  /// \memberof gfks_device_struct
  bool (*suitable_for_surface)(gfks_device *device, gfks_surface *surface);

  /// \public
  /// \brief Frees a device
  ///
  /// Must be called when you're done!
  /// \param device A device to be destroyed
  /// \memberof gfks_device_struct
  void (*free)(gfks_device *device);
};

/// \brief Obtains all graffiks-compatible devices
///
/// \param context A Graffiks context
/// \param devices_obtained Will be written with the number of devices returned.
/// \returns An array *gfks_device or NULL if there was an error.
/// \memberof gfks_device_struct
gfks_device *gfks_get_all_devices(gfks_context *context,
                                  uint32_t *devices_obtained);

/// \brief Obtains all graffiks-compatible devices that are suitable for drawing to the provided surfaces
///
/// \param context A Graffiks context
/// \param surfaces Surfaces we would like our devices to be suitable for
/// \param surface_count Number of surfaces you have passed to the surfaces parameter
/// \param devices_obtained Will be written with the number of devices returned.
/// \returns An array *gfks_device or NULL if there was an error.
/// \memberof gfks_device_struct
gfks_device*
gfks_get_devices_suitable_for_surfaces(gfks_context *context,
                                       gfks_surface *surfaces,
                                       uint32_t surface_count,
                                       uint32_t *devices_obtained);
void gfks_free_devices(gfks_device *devices, int device_count);


// -------------------
// --- gfks_shader ---
// -------------------

typedef struct gfks_shader_protected_struct gfks_shader_protected;

typedef enum {
  GFKS_SHADER_STAGE_VERTEX,
  GFKS_SHADER_STAGE_FRAGMENT,
  GFKS_SHADER_STAGE_GEOMETRY,
  GFKS_SHADER_STAGE_COMPUTE
} gfks_shader_stage;

/// gfks_shader
struct gfks_shader_struct {
  /// \private
  gfks_shader_protected* _protected;

  gfks_shader_stage shader_stage;

  gfks_device *device; // the device this shader is suitable for

  /// \public
  /// \brief Frees a shader
  ///
  /// Must be called when you're done
  /// \param device A shader to be destroyed
  /// \memberof gfks_shader_struct
  void (*free)(gfks_shader *shader);
};

gfks_shader* gfks_create_shader(void *SPIRV_data,
                                uint32_t SPIRV_data_size,
                                char *entry_func_name,
                                gfks_device *device,
                                gfks_shader_stage shader_stage);

// ------------------------
// --- gfks_render_pass ---
// ------------------------

typedef struct gfks_render_pass_protected_struct gfks_render_pass_protected;

static const uint32_t GFKS_MAX_RENDER_PASS_PRESENTATION_SURFACES = 256;

/// gfks_render_pass
struct gfks_render_pass_struct {
  /// \private
  gfks_render_pass_protected* _protected;

  gfks_device *device; // the device this render_pass will utilize
  gfks_context *context; // the parent context for this render pass

  /// \public
  /// \brief Frees a render_pass
  ///
  /// Must be called when you're done
  /// \param device A render_pass to be destroyed
  /// \memberof gfks_render_pass_struct
  void (*free)(gfks_render_pass *render_pass);

  // Surface must be capable of being drawn to by the device this render pass was created with
  // TODO Make sure we error if the surface can't be drawn to by the device
  // TODO have user set swapchain settings like double-buffer etc on surface add time?
  int16_t (*add_presentation_surface)(gfks_render_pass *render_pass,
                                      gfks_surface *surface); // returns index of surface or error -1
  bool (*remove_presentation_surface)(gfks_render_pass *render_pass, uint8_t index);

  // TODO methods to disable/enable presentation to specific presentation surfaces?
  uint32_t (*add_shader_set)(gfks_render_pass *render_pass, uint32_t shader_count, gfks_shader **shader_set);


  void (*set_shaderset_rasterization)(gfks_render_pass *render_pass,
                                      uint32_t shaderset_index,
                                      gfks_rasterization_settings *settings);
};

gfks_render_pass* gfks_create_render_pass(gfks_context *context, gfks_device *device, float width, float height);

// ------------------------
// --- gfks_render_plan ---
// ------------------------

typedef struct gfks_render_plan_protected_struct gfks_render_plan_protected;

/// gfks_render_plan
struct gfks_render_plan_struct {
  /// \private
  gfks_render_plan_protected* _protected;

  gfks_device *device; // the device this render_plan will utilize
  gfks_context *context; // the parent context for this render plan

  /// \public
  /// \brief Frees a render plan
  ///
  /// Must be called when you're done
  /// \param device A render_plan to be destroyed
  /// \memberof gfks_render_plan_struct
  void (*free)(gfks_render_plan *render_plan);
  void (*add_render_pass)(gfks_render_plan *plan, gfks_render_pass *pass);

  // Applies dependencies in our plan. Must be called again after future dependency changes.
  bool (*finalize)(gfks_render_plan *plan);

  //
  bool (*execute)(gfks_render_plan *plan);
};

gfks_render_plan* gfks_create_render_plan(gfks_context *context, gfks_device *device);



// -----------------------------------
// --- gfks_rasterization_settings ---
// -----------------------------------

typedef struct gfks_rasterization_settings_protected_struct gfks_rasterization_settings_protected;

/// gfks_rasterization_settings
struct gfks_rasterization_settings_struct {
  /// \private
  gfks_rasterization_settings_protected* _protected;

  /// \public
  /// \brief Frees a render plan
  ///
  /// Must be called when you're done
  /// \param device A rasterization_settings to be destroyed
  /// \memberof gfks_rasterization_settings_struct
  void (*free)(gfks_rasterization_settings *rasterization_settings);

  // TODO document.....
  void (*front_face_clockwise)(gfks_rasterization_settings *settings);
  void (*front_face_counterclockwise)(gfks_rasterization_settings *settings);
  void (*depth_clamp_enabled)(gfks_rasterization_settings *settings, bool setting);
  void (*rasterizer_discard_enabled)(gfks_rasterization_settings *settings, bool setting);
  void (*polygon_mode_fill)(gfks_rasterization_settings *settings);
  void (*polygon_mode_line)(gfks_rasterization_settings *settings);
  void (*polygon_mode_point)(gfks_rasterization_settings *settings);
  void (*line_width)(gfks_rasterization_settings *settings, float line_width);
  void (*disable_culling)(gfks_rasterization_settings *settings);
  void (*culling_front)(gfks_rasterization_settings *settings);
  void (*culling_back)(gfks_rasterization_settings *settings);
  void (*culling_frontback)(gfks_rasterization_settings *settings);
  void (*depth_bias_enabled)(gfks_rasterization_settings *settings, bool setting);
  void (*depth_bias_constant_factor)(gfks_rasterization_settings *settings, float setting);
  void (*depth_bias_clamp)(gfks_rasterization_settings *settings, float setting);
  void (*depth_bias_slope_factor)(gfks_rasterization_settings *settings, float setting);
};


gfks_rasterization_settings* gfks_create_rasterization_settings();
#endif
