#include "graffiks/internal.h"

#ifdef GFKS_CONFIG_X11_SUPPORT
#include <X11/X.h>     
#include <X11/Xlib.h> 
#endif


void gfks_free_surface(gfks_surface *surface) {
  vkDestroySurfaceKHR(*(surface->context->_protected->vk_instance), surface->_protected->vk_surface, NULL);
  free(surface->_protected->window_handle);
  free(surface->_protected);
  free(surface);
}

// Create a new malloc'd gfks_surface and assigns method pointers
//
// Argument window_handle must also be malloc'd as this object will be used in
// stack items below us
//
// This function will not fully complete the struct. vk_surface still needs to
// be defined
gfks_surface * make_surface(gfks_context *context,
                             void *window_handle,
                             gfks_window_system window_system) {

  gfks_surface *new_surface = malloc(sizeof(gfks_surface));
  new_surface->_protected = malloc(sizeof(gfks_surface_protected));
  new_surface->context = context;
  new_surface->_protected->window_handle = window_handle;
  new_surface->_protected->window_system = window_system;

  // Assign method pointers
  new_surface->free = &gfks_free_surface;

  if (new_surface == NULL || new_surface->_protected == NULL) {
    gfks_err(GFKS_ERROR_FAILED_MEMORY_ALLOCATION, 1, "Failed to allocate memory");
    return NULL;
  }
  
  return new_surface;
}

// TODO: should create surface for no window system (for invisible rendering)
gfks_surface* gfks_create_surface();

#ifdef GFKS_CONFIG_X11_SUPPORT
gfks_surface* gfks_create_surface_X11(gfks_context *context, Display *display, Window window) {
#if GFKS_DEBUG_LEVEL > 1
  printf("%s: Creating X11 surface\n",GFKS_DEBUG_TAG);
#endif

  if (context == NULL) {
    gfks_err(GFKS_ERROR_NULL_ARGUMENT, 1, "Received a NULL context for surface creation");
    return NULL;
  }

  if (display == NULL) {
    gfks_err(GFKS_ERROR_NULL_ARGUMENT, 1, "Received a NULL display for surface creation");
    return NULL;
  }

  // Set up our window handle
  window_handle_x11 *window_handle = malloc(sizeof(window_handle_x11));

  if (window_handle == NULL) {
    gfks_err(GFKS_ERROR_FAILED_MEMORY_ALLOCATION, 1, "Failed to allocate memory");
    free(window_handle);
    return NULL;
  }

  window_handle->display = display;
  window_handle->window = window;

  // Create the new surface
  gfks_surface *new_surface = make_surface(context,
                                           window_handle,
                                           GFKS_WINDOW_SYSTEM_X11_BITFLAG);

  if (new_surface == NULL) {
    // We produced an error in make_struct. Free and Return NULL.
    gfks_free_surface(new_surface);
    return NULL;
  }

  // Create our vulkan surface
  VkXlibSurfaceCreateInfoKHR xlib_surface_create_info = {};
  xlib_surface_create_info.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
  xlib_surface_create_info.pNext = NULL;
  xlib_surface_create_info.flags = 0;
  xlib_surface_create_info.dpy = display;
  xlib_surface_create_info.window = window;

  VkSurfaceKHR surface;
  if(vkCreateXlibSurfaceKHR(*(context->_protected->vk_instance), &xlib_surface_create_info, NULL, &surface) != VK_SUCCESS) {
    gfks_err(GFKS_ERROR_UNKNOWN, 1, "Failed to create vulkan surface");
    gfks_free_surface(new_surface);
    return NULL;
  }

  new_surface->_protected->vk_surface = surface;

  return new_surface;
}
#endif

// TODO: should create surface for wayland
gfks_surface* gfks_create_surface_wayland();


