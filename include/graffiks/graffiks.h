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
#define GFKS_MAJOR_VERSION 0
#define GFKS_MINOR_VERSION 0
#define GFKS_REVISION 0
#define GFKS_DEBUG_TAG "GFKS"

#ifndef GFKS_DEBUG_LEVEL
#define GFKS_DEBUG_LEVEL 0
#endif


// core public interface

// data types
typedef enum {
  GFKS_WINDOW_SYSTEM_NONE_BITFLAG = 1,
  GFKS_WINDOW_SYSTEM_X11_BITFLAG = 2,
  GFKS_WINDOW_SYSTEM_WAYLAND_BITFLAG = 4
} gfks_window_system;


// class definitions

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

/// gfks_surface
struct gfks_surface_struct {
  /// \private
  gfks_surface_protected *_protected;

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
///
/// \returns A Graffiks context. NULL if there was an error.
/// \memberof gfks_surface_struct
gfks_surface* gfks_create_surface();


#endif
