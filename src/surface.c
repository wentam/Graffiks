#include "graffiks/internal.h"

#ifdef GFKS_CONFIG_X11_SUPPORT
#include <X11/X.h>     
#include <X11/Xlib.h> 
#endif


void gfks_free_surface(gfks_surface *surface) {
  vkDestroySurfaceKHR(*(surface->context->_protected->vk_instance), *(surface->_protected->vk_surface), NULL);
  free(surface->_protected->window_handle);
  free(surface->_protected);
  free(surface);
}

// TODO: should create surface for no window system (for invisible rendering)
gfks_surface* gfks_create_surface();

#ifdef GFKS_CONFIG_X11_SUPPORT
gfks_surface* gfks_create_surface_X11(gfks_context *context, Display *display, Window window) {
  if (context == NULL) {
    gfks_err(GFKS_ERROR_NULL_CONTEXT, 1, "Received a NULL context for surface creation");
    return NULL;
  }

  // Allocate and set up surface struct
  gfks_surface *new_surface = malloc(sizeof(gfks_surface));
  new_surface->_protected = malloc(sizeof(gfks_surface_protected));
  new_surface->context = context;

  window_handle_x11 *window_handle = malloc(sizeof(window_handle_x11));

  if (new_surface == NULL || new_surface->_protected == NULL || window_handle == NULL) {
    gfks_err(GFKS_ERROR_FAILED_MEMORY_ALLOCATION, 1, "Failed to allocate memory");
    gfks_free_surface(new_surface);
    return NULL;
  }

  window_handle->display = display;
  window_handle->window = window;

  new_surface->_protected->window_handle = window_handle;

  // Assign method pointers
  new_surface->free = &gfks_free_surface;

  // Create our vulkan surface
  VkXlibSurfaceCreateInfoKHR xlib_surface_create_info = {};
  xlib_surface_create_info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
  xlib_surface_create_info.pNext = NULL;
  xlib_surface_create_info.flags = 0;
  xlib_surface_create_info.dpy = display;
  xlib_surface_create_info.window = window;

  if(vkCreateXlibSurfaceKHR(*(context->_protected->vk_instance), &xlib_surface_create_info, NULL, new_surface->_protected->vk_surface) != VK_SUCCESS) {
    gfks_err(GFKS_ERROR_UNKNOWN, 1, "Failed to create vulkan surface");
    gfks_free_surface(new_surface);
    return NULL;
  }

  return new_surface;
}
#endif

// TODO: should create surface for wayland
gfks_surface* gfks_create_surface_wayland();


