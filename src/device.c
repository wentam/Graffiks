#include "graffiks/internal.h"
#include "device_queue_utils.c"

void gfks_free_device(gfks_device *device) {
  vkDestroyDevice(device->_protected->vk_logical_device, NULL);
  free(device->_protected->vk_physical_device_properties);
  free(device->_protected->vk_physical_device_features);
  free(device->_protected->vk_queue_family_properties);
  free(device->_protected->queues);
  free(device->_protected->queue_families_for_queues);
  free(device->_protected->graphics_queue_indices);
  free(device->_protected->compute_queue_indices);
  free(device->_protected->transfer_queue_indices);
  free(device->_protected->sparse_binding_queue_indices);
  free(device->_protected);
  free(device);
}

void gfks_free_devices(gfks_device *devices, int device_count) {
  for (int i = 0; i < device_count; i++) {
    devices[i].free(&(devices[i]));
  }
}

static bool gfks_set_up_device_for_surfaces(gfks_device *device,
                                     gfks_surface *surfaces,
                                     uint32_t surface_count);

bool gfks_device_suitable_for_surface(gfks_device *device, gfks_surface *surface);

static void set_up_device_struct(gfks_device *device) {
  device->_protected = malloc(sizeof(gfks_device_protected));
  device->_protected->vk_physical_device_properties = NULL;
  device->_protected->vk_physical_device_features = NULL;
  device->_protected->vk_queue_family_properties = NULL;
  device->_protected->queues = NULL;
  device->_protected->queue_families_for_queues = NULL;
  device->_protected->graphics_queue_count = 0;
  device->_protected->graphics_queue_indices = NULL;
  device->_protected->compute_queue_count = 0;
  device->_protected->compute_queue_indices = NULL;
  device->_protected->transfer_queue_count = 0;
  device->_protected->transfer_queue_indices = NULL;
  device->_protected->sparse_binding_queue_count = 0;
  device->_protected->sparse_binding_queue_indices = NULL;
  device->_protected->presentation_queue_count = 0;
  device->_protected->presentation_queue_indices = NULL;

  // function pointers
  device->free = &gfks_free_device;
  device->set_up_for_surfaces = &gfks_set_up_device_for_surfaces;
  device->suitable_for_surface = &gfks_device_suitable_for_surface;
}

// Allocates our gfks_device structs and fills them with information about
// the devices
//
// Returns NULL on error
//
// If NULL is returned, we've already produced an error for the user of the
// engine
static gfks_device*
create_devices(VkInstance vk_instance,
               int *device_count) {
  // Enumerate physical devices
  uint32_t physical_device_count = 0;
  if (vkEnumeratePhysicalDevices(vk_instance,
                                 &physical_device_count,
                                 NULL) != VK_SUCCESS) {

    gfks_err(GFKS_ERROR_UNKNOWN_VULKAN,
             1,
             "Unkown vulkan error while enumerating physical devices");
    return NULL;
  }

  if (physical_device_count <= 0) {
    gfks_err(GFKS_ERROR_NO_VULKAN_DEVICE,
             1,
             "No vulkan compatible devices available");
    return NULL;
  }

#if GFKS_DEBUG_LEVEL > 0
  printf("%s: Found %i vulkan device(s)\n",
         GFKS_DEBUG_TAG, physical_device_count);
#endif

  *device_count = physical_device_count;

  VkPhysicalDevice physical_devices[physical_device_count];
  if (vkEnumeratePhysicalDevices(vk_instance,
                                 &physical_device_count,
                                 physical_devices) != VK_SUCCESS) {
    gfks_err(GFKS_ERROR_UNKNOWN_VULKAN,
             1,
             "Unkown vulkan error while enumerating physical devices");
    return NULL;
  } 

  // Allocate our gfks_devices for each physical device,
  gfks_device *gfks_devices = 
    malloc(sizeof(gfks_device)*physical_device_count);

  if (gfks_check_alloc_throw_err(gfks_devices,__FILE__,__LINE__)) return NULL;

  // Assign device properties to our devices
  for (int i = 0; i < physical_device_count; i++) {

    // Set up our device struct
    set_up_device_struct(&(gfks_devices[i]));


    VkPhysicalDeviceProperties *devprops =
      malloc(sizeof(VkPhysicalDeviceProperties)); 

    if (gfks_check_alloc_throw_err(devprops,__FILE__,__LINE__)) return NULL;

    vkGetPhysicalDeviceProperties(physical_devices[i], devprops);

    VkPhysicalDeviceFeatures *devfeat =
      malloc(sizeof(VkPhysicalDeviceFeatures));

    if (gfks_check_alloc_throw_err(devfeat,__FILE__,__LINE__)) return NULL;

    vkGetPhysicalDeviceFeatures(physical_devices[i], devfeat);

#if GFKS_DEBUG_LEVEL > 0
    printf("%s: - Device %i is a %s\n",GFKS_DEBUG_TAG,i,devprops->deviceName);
#endif

    gfks_devices[i]._protected->vk_physical_device = physical_devices[i];
    gfks_devices[i]._protected->vk_physical_device_index = i;
    gfks_devices[i]._protected->vk_physical_device_properties = devprops;
    gfks_devices[i]._protected->vk_physical_device_features = devfeat;

    if (apply_queue_family_info_to_device(&(gfks_devices[i])) == false) {
      return NULL;
    }
  }

  if (gfks_devices == NULL) {
    gfks_err(GFKS_ERROR_UNKNOWN,
             1,
             "Unknown error during device creation");

  }


  return gfks_devices;
}

// Decides what extensions to ask for at logical device creation time
const char * 
const *decide_device_extensions(gfks_device *device,
                                             unsigned int *count) {
  // TODO check if our device extensions are available 
  int required_device_extension_count = 1;
  char **required_device_extension_names =
    malloc(sizeof(char *)*required_device_extension_count);
  required_device_extension_names[0] =
    malloc(sizeof(char)*VK_MAX_EXTENSION_NAME_SIZE);
  required_device_extension_names[0] = "VK_KHR_swapchain";
  *count = required_device_extension_count;

  return (const char* const*) required_device_extension_names;
}

bool create_logical_device(gfks_device *device,
                           const char * const*device_extensions,
                           int device_extension_count,
                           queue_family_decision *queue_family_decisions,
                           int queue_family_count) {

  // Create queue create info array
  int used_queue_families = 0; 
  for (int i = 0; i < queue_family_count; i++) {
    queue_family_decision d = queue_family_decisions[i];
    if (d.queue_count > 0) {
      used_queue_families++;
    }
  }

  VkDeviceQueueCreateInfo queue_create_info[used_queue_families];
  int j = 0;
  for (int i = 0; i < queue_family_count; i++) {  
    queue_family_decision d = queue_family_decisions[i];
    if (d.queue_count > 0) {
      queue_create_info[j++] = d.family_queue_create_info;
    }
  }

  // set up logical device
  VkPhysicalDeviceFeatures device_features = {};

  VkDeviceCreateInfo device_create_info = {};
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_create_info.pQueueCreateInfos = queue_create_info;
  device_create_info.queueCreateInfoCount = used_queue_families;
  device_create_info.pEnabledFeatures = &device_features;
  device_create_info.enabledExtensionCount = device_extension_count;
  device_create_info.ppEnabledExtensionNames = device_extensions;
  device_create_info.enabledLayerCount = 0;

  VkDevice ldevice;
  if (vkCreateDevice(device->_protected->vk_physical_device,
                     &device_create_info,
                     NULL,
                     &ldevice) != VK_SUCCESS) {

    gfks_err(GFKS_ERROR_UNKNOWN_VULKAN,
             1,
             "Failed to create logical vulkan device for unknown reason");
    return false;
  }
  device->_protected->vk_logical_device = ldevice; 

  return true;
}

// TODO check for other types of suitability
bool gfks_device_suitable_for_surface(gfks_device *device, gfks_surface *surface) {
  for (int i = 0; i < device->_protected->presentation_queue_count; i++) {
    int queue_family = device->_protected->queue_families_for_queues[
        device->_protected->presentation_queue_indices[i]
      ];

    VkBool32 presentSupport = False;                                                          
    vkGetPhysicalDeviceSurfaceSupportKHR(device->_protected->vk_physical_device,
                                         queue_family,
                                         surface->_protected->vk_surface,
                                         &presentSupport);

    if (presentSupport == True) {
      return true;
    }
  }
}

static bool gfks_set_up_device_for_surfaces(gfks_device *device,
                                     gfks_surface *surfaces,
                                     uint32_t surface_count) {

  // Decide what queues to create
  unsigned int queue_family_count;

  queue_family_decision *queue_family_decisions =
    decide_queues(device, surfaces, surface_count, &queue_family_count);

#if GFKS_DEBUG_LEVEL > 1
  for (int j = 0; j < queue_family_count; j++)  {
    for (int k = 0; k < queue_family_decisions[j].queue_count; k++) {
      queue_purpose p = queue_family_decisions[j].queue_purposes[k];
      printf("%s: Decided to create queue in queue family %i with purpose 0x%08x\n",
             GFKS_DEBUG_TAG,
             j,
             p);
    }
  }
#endif

  unsigned int device_extension_count;
  const char * const* extension_decisions =
    decide_device_extensions(device, &device_extension_count);

  if (create_logical_device(device,
                            extension_decisions,
                            device_extension_count,
                            queue_family_decisions,
                            queue_family_count) == false) {
    // We produce errors for this within the function. return false.
    return false;
  }

  create_queues(device,
                queue_family_decisions,
                queue_family_count); 

  free_queue_family_decisions(queue_family_decisions, queue_family_count);
}

gfks_device *gfks_get_all_devices(gfks_context *context,
                                  uint32_t *devices_obtained) {

  int device_count;
  gfks_device *gfks_devices = 
    create_devices(*(context->_protected->vk_instance),
                   &device_count);

  if (gfks_devices == NULL) {
    // We produce errors for this inside create_devices. return NULL.
    return NULL;
  }

  if (devices_obtained != NULL) {
    *devices_obtained = device_count; 
  }

  return gfks_devices;
}

// Outputs an array of devices suitable for rendering graphics and presenting
// to the provided surface
gfks_device*
gfks_get_devices_suitable_for_surfaces(gfks_context *context,
                                       gfks_surface *surfaces,
                                       uint32_t surface_count,
                                       uint32_t *devices_obtained) {

#if GFKS_DEBUG_LEVEL > 1
  printf("%s: Getting devices suitable for surface.\n",GFKS_DEBUG_TAG);
#endif

  // Create our devices
  int device_count;

  gfks_device *gfks_devices = 
    create_devices(*(context->_protected->vk_instance),
                   &device_count);

  if (gfks_devices == NULL) {
    // We produce errors for this inside create_devices. return NULL.
    return NULL;
  }

  // Figure out what queues to create for each device, 
  // figure out what extensions to enable,
  // create our logical vulkan device,
  // and create queues for it
  for (int i = 0; i < device_count; i++) {
    gfks_set_up_device_for_surfaces(&(gfks_devices[i]),surfaces,surface_count);
  }

  // TODO test devices for suitability and create potentially shorter output
  // array
  if (devices_obtained != NULL) {
    *devices_obtained = device_count; 
  }
  return gfks_devices;
}
