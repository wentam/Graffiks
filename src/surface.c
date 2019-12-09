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

bool gfks_set_surface_draw_device(gfks_surface *surface, gfks_device *device) {

  // TODO init all device-dependant stuff
  // Validate input
  if (device == NULL) {
    // TODO error
    return false;
  }

  if (surface == NULL) {
    // TODO error
    return false;
  }

  // Ensure the passed device is suitable for our surface
  if (!(device->suitable_for_surface(device, surface))) {
    // TODO error
    return false;
  }

  // Set our draw device
  surface->_protected->draw_device = device;  

  // Get surface capabilities
  VkSurfaceCapabilitiesKHR capabilities;                                                                                                                  
  if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->_protected->vk_physical_device,
                                                surface->_protected->vk_surface,
                                                &capabilities) != VK_SUCCESS){
    // TODO error
    return false;
  }

#if GFKS_DEBUG_LEVEL > 0
  printf("%s: Surface capabilities:\n",GFKS_DEBUG_TAG);   
  printf("%s: - minimum image count:%i\n",GFKS_DEBUG_TAG,capabilities.minImageCount);
  printf("%s: - maximum image count:%i\n",GFKS_DEBUG_TAG,capabilities.maxImageCount);
  printf("%s: - current extent width:%i\n",GFKS_DEBUG_TAG,capabilities.currentExtent.width);
  printf("%s: - current extent height:%i\n",GFKS_DEBUG_TAG,capabilities.currentExtent.height);
  printf("%s: - minimum extent width:%i\n",GFKS_DEBUG_TAG,capabilities.minImageExtent.width);
  printf("%s: - minimum extent height:%i\n",GFKS_DEBUG_TAG,capabilities.minImageExtent.height);
  printf("%s: - maximum extent width:%i\n",GFKS_DEBUG_TAG,capabilities.maxImageExtent.width);
  printf("%s: - maximum extent height:%i\n",GFKS_DEBUG_TAG,capabilities.maxImageExtent.height);
#endif


  surface->_protected->capabilities = capabilities;

  // Create our swap chain as close as possible in size to our window size,
  // but within the capabilities of the surface
  int width = surface->_protected->window_width; 
  int height = surface->_protected->window_height;
  if (width < capabilities.minImageExtent.width) {
    width = capabilities.minImageExtent.width;
  } else if (width > capabilities.maxImageExtent.width) {
    width = capabilities.maxImageExtent.width;
  }

  if (height < capabilities.minImageExtent.height) {
    height = capabilities.minImageExtent.height;
  } else if (height > capabilities.maxImageExtent.height) {
    height = capabilities.maxImageExtent.height;
  }

  VkExtent2D swap_chain_extent;
  swap_chain_extent.width = width;
  swap_chain_extent.height = height;

  surface->_protected->swap_chain_extent = swap_chain_extent;

#if GFKS_DEBUG_LEVEL > 0
  printf("%s: Decided to use swap chain extent of size %ix%i\n",GFKS_DEBUG_TAG,width,height);
#endif

  // Decide how many swap chain images we would like to use.
  const uint32_t desired_swap_chain_images = 2;  // TODO should not be static

  // Ensure that the number of swap chains is within the capabilities of the
  // surface.
  uint32_t swap_chain_image_count = desired_swap_chain_images;

  if (swap_chain_image_count < capabilities.minImageCount) {
    swap_chain_image_count = capabilities.minImageCount;
  } else if (swap_chain_image_count > capabilities.maxImageCount) {
    swap_chain_image_count = capabilities.maxImageCount;
  }

#if GFKS_DEBUG_LEVEL > 0
  printf("%s: Decided to use swap chain image count %i\n",GFKS_DEBUG_TAG,swap_chain_image_count);
#endif

  surface->_protected->swap_chain_image_count = swap_chain_image_count;

  // Choose a surface format
  // TODO surface format is probably something we want to give the user of the
  // engine some level of control over.
  // 
  // I'm unclear on exactly how this works -- do shaders need to output colors in
  // the chosen format? I'm guessing yes, and if so, built-in shaders could only
  // be produced for some color formats.

  uint32_t format_count;
  if (vkGetPhysicalDeviceSurfaceFormatsKHR(device->_protected->vk_physical_device,
                                           surface->_protected->vk_surface,
                                           &format_count, NULL) != VK_SUCCESS) {
    // TODO error
    return false;
  }

#if GFKS_DEBUG_LEVEL > 0
  printf("%s: Got %i device surface formats\n",GFKS_DEBUG_TAG, format_count);
#endif

  VkSurfaceFormatKHR formats[format_count];                                                                        
  if (vkGetPhysicalDeviceSurfaceFormatsKHR(device->_protected->vk_physical_device,
                                           surface->_protected->vk_surface,
                                           &format_count,
                                           formats) != VK_SUCCESS) {
    // TODO error
    return false;
  }

#if GFKS_DEBUG_LEVEL > 0
  for (int i = 0; i < format_count; i++) {
    VkSurfaceFormatKHR f = formats[i];
    printf("%s: - Surface format %i has VkFormat enum value %i\n",
           GFKS_DEBUG_TAG,
           i,
           f.format);
    printf("%s: - Surface format %i has VkColorSpaceKHR enum value %i\n",
           GFKS_DEBUG_TAG,
           i,
           f.colorSpace);
  }
#endif


  // Currently, we choose the first format with an sRGB colorspace
  // TODO: we should define preferences regarding which vkFormat we get.
  int chosen_format_index = -1;
  for (int i = 0; i < format_count; i++) {
    VkSurfaceFormatKHR f = formats[i];
     if (f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        chosen_format_index = i;
        break;
     }
  }

  if (chosen_format_index == -1) {
    // TODO error
    return false; 
  }

#if GFKS_DEBUG_LEVEL > 0
  printf("%s: Decided to use surface format %i\n",GFKS_DEBUG_TAG,chosen_format_index);
#endif

  surface->_protected->swap_chain_image_format =
    formats[chosen_format_index].format;
  surface->_protected->surface_format = formats[chosen_format_index];

  // Choose a surface presentation mode
  uint32_t present_mode_count;
  if (vkGetPhysicalDeviceSurfacePresentModesKHR(device->_protected->vk_physical_device,
                                                surface->_protected->vk_surface,
                                                &present_mode_count,
                                                NULL) != VK_SUCCESS) {
    // TODO error
    return false;
  }

#if GFKS_DEBUG_LEVEL > 0
  printf("%s: Got %i surface presentation modes\n",GFKS_DEBUG_TAG,present_mode_count);
#endif

  VkPresentModeKHR present_modes[present_mode_count];
  if (vkGetPhysicalDeviceSurfacePresentModesKHR(device->_protected->vk_physical_device,
                                                surface->_protected->vk_surface,
                                                &present_mode_count,
                                                present_modes) != VK_SUCCESS){
    // TODO error
    return false;
  }

#if GFKS_DEBUG_LEVEL > 0
  for (int i = 0; i < present_mode_count; i++) {
    printf("%s: - Presentation mode %i has VkPresentModeKHR enum value %i\n",
           GFKS_DEBUG_TAG,
           i,
           present_modes[i]);
  }
#endif

  // TODO support other present modes beyond immediate
  // the user of the engine should probably tell us they want "double buffering"
  // or "triple buffering" etc.
  int chosen_present_mode = -1;
  for (int i = 0; i < present_mode_count; i++) {
   if (present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
     chosen_present_mode = i;
     break;
   }
  }

#if GFKS_DEBUG_LEVEL > 0
  printf("%s: Decided to use presentation mode %i\n",GFKS_DEBUG_TAG,chosen_present_mode);
#endif

  surface->_protected->present_mode = present_modes[chosen_present_mode];

  // Create swap chain
  VkSwapchainCreateInfoKHR swap_chain_create_info = {};
  swap_chain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swap_chain_create_info.surface = surface->_protected->vk_surface;
  swap_chain_create_info.minImageCount = surface->_protected->swap_chain_image_count;

  swap_chain_create_info.imageFormat = surface->_protected->surface_format.format;
  swap_chain_create_info.imageColorSpace = surface->_protected->surface_format.colorSpace;

  swap_chain_create_info.imageExtent = surface->_protected->swap_chain_extent;
  swap_chain_create_info.imageArrayLayers = 1;

  swap_chain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swap_chain_create_info.queueFamilyIndexCount = 0;
  swap_chain_create_info.pQueueFamilyIndices = NULL;
  swap_chain_create_info.preTransform = surface->_protected->capabilities.currentTransform;
  swap_chain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swap_chain_create_info.presentMode = surface->_protected->present_mode;
  swap_chain_create_info.clipped = VK_TRUE;

  swap_chain_create_info.oldSwapchain = VK_NULL_HANDLE;

  VkSwapchainKHR swap_chain;
  if (vkCreateSwapchainKHR(device->_protected->vk_logical_device,
                           &swap_chain_create_info,
                           NULL,
                           &swap_chain) != VK_SUCCESS) {
    // TODO error
    return false;
  }

  surface->_protected->swap_chain = swap_chain;

  // Get swap chain images
  uint32_t nswap_chain_image_count;
  if (vkGetSwapchainImagesKHR(device->_protected->vk_logical_device,
                              surface->_protected->swap_chain,
                              &nswap_chain_image_count,
                              NULL) != VK_SUCCESS) {
    // TODO error
    return false;
  }

#if GFKS_DEBUG_LEVEL > 0
  printf("%s: Got %i images from swap chain\n",
         GFKS_DEBUG_TAG,nswap_chain_image_count);
#endif

  // TODO free this on surface free
  VkImage *swap_chain_images = malloc(sizeof(VkImage)*nswap_chain_image_count);
  if (vkGetSwapchainImagesKHR(device->_protected->vk_logical_device,
                              surface->_protected->swap_chain,
                              &nswap_chain_image_count,
                              swap_chain_images) != VK_SUCCESS) {
    // TODO error
    return false;
  }

  surface->_protected->swap_chain_images = swap_chain_images;
  surface->_protected->swap_chain_image_count = nswap_chain_image_count;

  // Create swap chain image views
  // TODO free this on surface free
  VkImageView *swap_chain_image_views = malloc(sizeof(VkImageView)*swap_chain_image_count);

  for (int i = 0; i < swap_chain_image_count; i++) {
    VkImageViewCreateInfo ci = {};
    ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ci.image = surface->_protected->swap_chain_images[i];
    ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
    ci.format = surface->_protected->swap_chain_image_format;
    ci.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    ci.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    ci.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    ci.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    ci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    ci.subresourceRange.baseMipLevel = 0;
    ci.subresourceRange.levelCount = 1;
    ci.subresourceRange.baseArrayLayer = 0;
    ci.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device->_protected->vk_logical_device,
                          &ci,
                          NULL,
                          &swap_chain_image_views[i]) != VK_SUCCESS) {
      // TODO error
      return false;
    }
  }

  surface->_protected->swap_chain_image_views = swap_chain_image_views;

  return true;
}

static gfks_surface *init_struct() {
  gfks_surface *new_surface = malloc(sizeof(gfks_surface));
  new_surface->_protected = malloc(sizeof(gfks_surface_protected));
  new_surface->_protected->window_handle = NULL;
  new_surface->_protected->draw_device = NULL;
  new_surface->_protected->swap_chain_images = NULL;

  // Assign method pointers
  new_surface->free = &gfks_free_surface;
  new_surface->set_draw_device = &gfks_set_surface_draw_device;

  return new_surface;
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

  gfks_surface *new_surface = init_struct();
  new_surface->context = context;
  new_surface->_protected->window_handle = window_handle;
  new_surface->_protected->window_system = window_system;

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


