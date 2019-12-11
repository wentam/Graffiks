#include "graffiks/internal.h"

#ifdef GFKS_CONFIG_X11_SUPPORT
#include <X11/X.h>
#include <X11/Xlib.h>
#endif

// TODO multi-gpu support?

// Frees a gfks_surface
void gfks_free_surface(gfks_surface *surface) {
/*  if (surface->_protected->swap_chain_defined) {
    vkDestroySwapchainKHR(surface->draw_device->_protected->vk_logical_device,
                          surface->_protected->swap_chain,
                          NULL);
  }*/

  vkDestroySurfaceKHR(*(surface->context->_protected->vk_instance),
                      surface->_protected->vk_surface,
                      NULL);

  free(surface->_protected->window_handle);
  free(surface->_protected);
  free(surface);
}

// Allocates memory for our struct,
// assigns NULL to all pointer members,
// initializes primitive members,
// assigns method pointers,
// and returns the struct.
static gfks_surface *init_struct() {
  // Allocate memory for our struct
  gfks_surface *new_surface = malloc(sizeof(gfks_surface));
  new_surface->_protected = malloc(sizeof(gfks_surface_protected));

  if (new_surface == NULL || new_surface->_protected == NULL) {
    gfks_err(GFKS_ERROR_FAILED_MEMORY_ALLOCATION, 1, "Failed to allocate memory");
    return NULL;
  }

  // Assign NULL to all pointer members
  new_surface->_protected->window_handle          = NULL;

  // Assign method pointers
  new_surface->free            = &gfks_free_surface;

  // Return the struct
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
  gfks_surface *new_surface = init_struct();

  new_surface->context = context;
  new_surface->_protected->window_handle = window_handle;
  new_surface->_protected->window_system = GFKS_WINDOW_SYSTEM_X11_BITFLAG;

  // get window geometry
  Window root;
  int window_x;
  int window_y;
  unsigned int window_width;
  unsigned int window_height;
  unsigned int border_width;
  unsigned int depth;
  XGetGeometry(display, window, &root, &window_x, &window_y, &window_width,
               &window_height, &border_width, &depth);

  new_surface->_protected->window_x = window_x;
  new_surface->_protected->window_y = window_y;
  new_surface->_protected->window_width = window_width;
  new_surface->_protected->window_height = window_height;

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
  if(vkCreateXlibSurfaceKHR(*(context->_protected->vk_instance),
                            &xlib_surface_create_info,
                            NULL,
                            &surface) != VK_SUCCESS) {
    gfks_err(GFKS_ERROR_UNKNOWN, 1, "Failed to create vulkan surface");
    gfks_free_surface(new_surface);
    return NULL;
  }

  // Assign surface to our struct
  new_surface->_protected->vk_surface = surface;

  return new_surface;
}
#endif

// TODO: should create surface for wayland
gfks_surface* gfks_create_surface_wayland();
