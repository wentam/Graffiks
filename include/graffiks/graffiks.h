/// \file

#ifndef GFKS_DRIVER_H
#define GFKS_DRIVER_H

#ifndef DLL_EXPORT
#ifdef _WIN32
#ifdef GRAFFIKS_BUILD_SHARED
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif
#else
#define DLL_EXPORT
#endif
#endif

#include <stdbool.h>

#ifdef _WIN32
#include "graffiks/dt_loop.h"
#include "graffiks/dt_callbacks.h"
#include <windows.h>
#endif

#ifdef LINUX
#include <GL/glx.h>
#endif

#ifdef _WIN32
HDC hdc;
#endif

#ifdef LINUX
Display *display;
Window win;
#endif

DLL_EXPORT void gfks_init_dt(int window_width, int window_height, char *window_title,
                             void (*init)(int *width, int *height),
                             void (*update)(float time_step), void (*finish)(void));

#ifdef LINUX
DLL_EXPORT void gfks_init_with_window(Display *display, Window window);
DLL_EXPORT void gfks_init_with_window_dt(Display *display, Window window,
                                         void (*init)(int *width, int *height),
                                         void (*update)(float time_step), void (*finish)(void));
#endif

#ifdef _WIN32
// windows version of init_with_window and init_with_window_dt
#endif

DLL_EXPORT void gfks_init(int window_width, int window_height, char *window_title);

DLL_EXPORT void gfks_use_vsync(int vsync);

DLL_EXPORT void gfks_set_antialiasing_samples(int samples);

// ----------------------------- old stuff above (deprecated) ------------------
// ----------------------------- new stuff below -------------------------------

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
} gfks_error;

gfks_error gfks_latest_error;

// Data types
typedef enum {
  GFKS_WINDOW_SYSTEM_NONE_BITFLAG = 1,
  GFKS_WINDOW_SYSTEM_X11_BITFLAG = 2,
  GFKS_WINDOW_SYSTEM_WAYLAND_BITFLAG = 4
} gfks_window_system;


// Class definitions

// --------------------
// --- gfks_context ---
// --------------------

typedef struct gfks_context_protected_struct gfks_context_protected;
typedef struct gfks_context_struct gfks_context;

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

typedef struct gfks_surface_protected_struct gfks_surface_protected;
typedef struct gfks_surface_struct gfks_surface;
typedef struct gfks_device_struct gfks_device;

/// gfks_surface
struct gfks_surface_struct {
  /// \private
  gfks_surface_protected *_protected;

  /// \public
  /// \brief Read only. The parent Graffiks context.
  /// \memberof gfks_surface_struct
  gfks_context *context;

  /// \public
  /// \brief Read only, use set_draw_device. The device we'll use when drawing to this surface.
  /// \memberof gfks_surface_struct
  gfks_device *draw_device;

  /// \public
  /// \brief Sets which device will be used to draw to this surface
  ///
  /// \param gfks_surface The surface we want this device to draw to
  /// \param gfks_device The device we want to use to draw to this surface
  /// \returns true on success, false on error - look at gfks_latest_error to get more information.
  /// \memberof gfks_surface_struct
  bool (*set_draw_device)(gfks_surface *surface, gfks_device *device);

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
#endif
