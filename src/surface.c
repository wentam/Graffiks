#include "graffiks/internal.h"


void gfks_free_surface(gfks_surface *surface) {
  free(surface->_protected->window_handle);
  free(surface->_protected);
  free(surface);
}

// TODO: should create surface for no window system (for invisible rendering)
gfks_surface* gfks_create_surface() {

}

gfks_surface* gfks_create_surface_X11(gfks_context *context, Display *display, Window window) {
  // Allocate and set up surface struct
  gfks_surface *new_surface = malloc(sizeof(gfks_surface)); 
  new_surface->_protected = malloc(sizeof(gfks_surface_protected));

  window_handle_x11 *window_handle = malloc(sizeof(window_handle_x11));

  if (new_surface == NULL || new_surface->_protected == NULL || window_handle == NULL) {
    // TODO: memory allocation error, throw error before returning
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
    // TODO throw error before returning
    return NULL;
  }

  return new_surface;
}


// TODO: should create surface for wayland
gfks_surface* gfks_create_surface_wayland();


