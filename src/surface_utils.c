#include "graffiks/internal.h"

// Outputs a swap chain for the passed surface that the passed device can draw to
//
// Mallocs, with you being responsible to free:
// swap chain
// swap chain image views
//
// Outputs:
// swap chain
// swap chain image count
// swap chain images
// swap chain image views
//
// Returns true on success, false on error
//
// Will produce Graffiks error(s) on error
static bool gfks_surface_util_create_swap_chain(gfks_surface             *surface,
                                                gfks_device              *device,
                                                VkSurfaceCapabilitiesKHR capabilities,
                                                VkSurfaceFormatKHR       format,
                                                VkPresentModeKHR         present_mode,
                                                VkExtent2D               swap_chain_extent,
                                                VkSwapchainKHR           *output_swap_chain,
                                                uint32_t                 *output_swap_chain_image_count,
                                                VkImage                  **output_swap_chain_images,
                                                VkImageView              **output_swap_chain_image_views,
                                                uint32_t                 desired_swap_chain_image_count) {

  // Create swap chain
  VkSwapchainCreateInfoKHR swap_chain_create_info = {};
  swap_chain_create_info.sType   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swap_chain_create_info.surface = surface->_protected->vk_surface;

  swap_chain_create_info.minImageCount    = desired_swap_chain_image_count;
  swap_chain_create_info.imageFormat      = format.format;
  swap_chain_create_info.imageColorSpace  = format.colorSpace;
  swap_chain_create_info.imageExtent      = swap_chain_extent;
  swap_chain_create_info.imageArrayLayers = 1;
  swap_chain_create_info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

  swap_chain_create_info.queueFamilyIndexCount = 0;
  swap_chain_create_info.pQueueFamilyIndices   = NULL;

  swap_chain_create_info.preTransform   = capabilities.currentTransform;
  swap_chain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swap_chain_create_info.presentMode    = present_mode;
  swap_chain_create_info.clipped        = VK_TRUE;

  swap_chain_create_info.oldSwapchain = VK_NULL_HANDLE;

  VkSwapchainKHR swap_chain;
  if (vkCreateSwapchainKHR(device->_protected->vk_logical_device,
                           &swap_chain_create_info,
                           NULL,
                           &swap_chain) != VK_SUCCESS) {
    gfks_err(GFKS_ERROR_UNKNOWN_VULKAN, 1, "Vulkan error");
    return false;
  }

  // Output this swap chain
  *output_swap_chain = swap_chain;

  // Get swap chain images
  uint32_t swap_chain_image_count;
  if (vkGetSwapchainImagesKHR(device->_protected->vk_logical_device,
                              swap_chain,
                              &swap_chain_image_count,
                              NULL) != VK_SUCCESS) {
    gfks_err(GFKS_ERROR_UNKNOWN_VULKAN, 1, "Vulkan error");
    return false;
  }

#if GFKS_DEBUG_LEVEL > 0
  printf("%s: Got %i images from swap chain\n",
         GFKS_DEBUG_TAG,swap_chain_image_count);
#endif

  // TODO free this on surface free
  VkImage *swap_chain_images = malloc(sizeof(VkImage)*swap_chain_image_count);

  if (swap_chain_images == NULL) {
    gfks_err(GFKS_ERROR_FAILED_MEMORY_ALLOCATION, 1, "Failed to allocate memory");
    return false;
  }

  if (vkGetSwapchainImagesKHR(device->_protected->vk_logical_device,
                              swap_chain,
                              &swap_chain_image_count,
                              swap_chain_images) != VK_SUCCESS) {
    gfks_err(GFKS_ERROR_UNKNOWN_VULKAN, 1, "Vulkan error");
    return false;
  }

  // Output swap chain images
  *output_swap_chain_images = swap_chain_images;
  *output_swap_chain_image_count = swap_chain_image_count;

  // Create swap chain image views
  VkImageView *swap_chain_image_views = malloc(sizeof(VkImageView)*swap_chain_image_count);

  if (swap_chain_image_views == NULL) {
    gfks_err(GFKS_ERROR_FAILED_MEMORY_ALLOCATION, 1, "Failed to allocate memory");
    return false;
  }

  for (int i = 0; i < swap_chain_image_count; i++) {
    VkImageViewCreateInfo ci = {};
    ci.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ci.image    = swap_chain_images[i];
    ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
    ci.format   = format.format;

    ci.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    ci.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    ci.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    ci.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    ci.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    ci.subresourceRange.baseMipLevel   = 0;
    ci.subresourceRange.levelCount     = 1;
    ci.subresourceRange.baseArrayLayer = 0;
    ci.subresourceRange.layerCount     = 1;

    if (vkCreateImageView(device->_protected->vk_logical_device,
                          &ci,
                          NULL,
                          &swap_chain_image_views[i]) != VK_SUCCESS) {
      gfks_err(GFKS_ERROR_UNKNOWN_VULKAN, 1, "Vulkan error");
      return false;
    }
  }

  // Output image views
  *output_swap_chain_image_views = swap_chain_image_views;
  printf("FFFFFOOOOOOOO %p\n",swap_chain_image_views);

  return true;
}

// Outputs a surface format for this device
//
// Returns false on error, otherwise true
//
// Will produce Graffiks error(s) on error
static bool gfks_surface_util_decide_surface_format_for_device(gfks_surface *surface,
                                                               gfks_device *device,
                                                               VkSurfaceFormatKHR *output) {
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
    gfks_err(GFKS_ERROR_UNKNOWN_VULKAN, 1, "Vulkan error");
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
    gfks_err(GFKS_ERROR_UNKNOWN_VULKAN, 1, "Vulkan error");
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
    gfks_err(GFKS_ERROR_NO_SUITABLE_SURFACE_FORMAT, 1, "Failed to find suitable surface format");
    return false;
  }

#if GFKS_DEBUG_LEVEL > 0
  printf("%s: Decided to use surface format %i\n",GFKS_DEBUG_TAG,chosen_format_index);
#endif

  *output = formats[chosen_format_index];
  return true;
}

// Returns a swap chain extent for this surface that is within the provided surface capabilities
static VkExtent2D gfks_surface_util_decide_swap_chain_image_size(VkSurfaceCapabilitiesKHR capabilities,
                                               gfks_surface *surface) {

  // Our extent size should be as close as possible in size to our window,  but within the
  // capabilities of the surface

  int width = surface->_protected->window_width;
  int height = surface->_protected->window_height;

  if (width < capabilities.minImageExtent.width) {width = capabilities.minImageExtent.width;} 
  else if (width > capabilities.maxImageExtent.width) {width = capabilities.maxImageExtent.width;}

  if (height < capabilities.minImageExtent.height) {height = capabilities.minImageExtent.height;}
  else if (height > capabilities.maxImageExtent.height) {height = capabilities.maxImageExtent.height;}

  VkExtent2D swap_chain_extent;
  swap_chain_extent.width = width;
  swap_chain_extent.height = height;


#if GFKS_DEBUG_LEVEL > 0
  printf("%s: Decided to use swap chain extent of size %ix%i\n",GFKS_DEBUG_TAG,width,height);
#endif

  return swap_chain_extent;
}

// Outputs the capabilities of the surface when used with the provided device.
//
// Returns true on success, false on error.
//
// Will produce Graffiks error(s) on error
static bool gfks_surface_util_obtain_surface_capabilities_for_device(gfks_surface *surface,
                                                                     gfks_device *device,
                                                                     VkSurfaceCapabilitiesKHR *output) {

  VkSurfaceCapabilitiesKHR capabilities;
  if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->_protected->vk_physical_device,
                                                surface->_protected->vk_surface,
                                                &capabilities) != VK_SUCCESS) {
    gfks_err(GFKS_ERROR_UNKNOWN_VULKAN, 1, "Vulkan error");
    return false;
  }

  *output = capabilities;
  return true;
}

// Returns the number of swap chain images we've decided to use within the limitations of our
// surface capabilities.
static uint32_t gfks_surface_util_decide_desired_swap_chain_image_count(VkSurfaceCapabilitiesKHR capabilities) {
  uint32_t desired_swap_chain_image_count = 2;  // TODO should not be static

  // Ensure that the number of swap chains is within the capabilities of the surface.
  // We're still not guaranteed to get this number of images, so this is still a "desired" count.
  if (desired_swap_chain_image_count < capabilities.minImageCount) {
    desired_swap_chain_image_count = capabilities.minImageCount;
  } else if (desired_swap_chain_image_count > capabilities.maxImageCount) {
    desired_swap_chain_image_count = capabilities.maxImageCount;
  }

#if GFKS_DEBUG_LEVEL > 0
  printf("%s: Decided we want use swap chain image count %i\n",
         GFKS_DEBUG_TAG,
         desired_swap_chain_image_count);
#endif

  return desired_swap_chain_image_count;
}


// Outputs a presentation mode
//
// Returns true on success, false on error
//
// Will produce Graffiks error(s) on error
static bool gfks_surface_util_decide_surface_presentation_mode_for_device(gfks_surface     *surface,
                                                                   gfks_device      *device,
                                                                   VkPresentModeKHR *output_present_mode) {

  uint32_t present_mode_count;
  if (vkGetPhysicalDeviceSurfacePresentModesKHR(device->_protected->vk_physical_device,
                                                surface->_protected->vk_surface,
                                                &present_mode_count,
                                                NULL) != VK_SUCCESS) {
    gfks_err(GFKS_ERROR_UNKNOWN_VULKAN, 1, "Vulkan error");
    return false;
  }

#if GFKS_DEBUG_LEVEL > 0
  printf("%s: Got %i surface presentation modes\n",GFKS_DEBUG_TAG,present_mode_count);
#endif

  VkPresentModeKHR present_modes[present_mode_count];
  if (vkGetPhysicalDeviceSurfacePresentModesKHR(device->_protected->vk_physical_device,
                                                surface->_protected->vk_surface,
                                                &present_mode_count,
                                                present_modes) != VK_SUCCESS) {
    gfks_err(GFKS_ERROR_UNKNOWN_VULKAN, 1, "Vulkan error");
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

  *output_present_mode = present_modes[chosen_present_mode];

  return true;
}
