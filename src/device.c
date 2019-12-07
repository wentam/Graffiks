#include "graffiks/internal.h"

// Allocates our gfks_device structs and fills them with information about
// the devices
static gfks_device** make_devs_from_physical_dev_enumeration(VkInstance vk_instance,
                                                             int *device_count) {
  // Enumerate physical devices
  uint32_t physical_device_count = 0;
  if (vkEnumeratePhysicalDevices(vk_instance,
                                 &physical_device_count,
                                 NULL) != VK_SUCCESS) {
    // TODO error
  }

  if (physical_device_count <= 0) {
    // TODO error
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
    // TODO error
  } 

  // Allocate our gfks_devices for each physical device,
  // and assign the device properties to them
  gfks_device **gfks_devices = 
    malloc(sizeof(gfks_device *)*physical_device_count);

  if (gfks_devices == NULL) {
    // TODO error
  }

  for (int i = 0; i < physical_device_count; i++) {
    gfks_devices[i] = malloc(sizeof(gfks_device));

    if (gfks_devices[i] == NULL) {
      // TODO error
    }
    
    VkPhysicalDeviceProperties *devprops =
      malloc(sizeof(VkPhysicalDeviceProperties)); 

    if (devprops == NULL) {
      // TODO error
    }

    vkGetPhysicalDeviceProperties(physical_devices[i], devprops);

    VkPhysicalDeviceFeatures *devfeat =
      malloc(sizeof(VkPhysicalDeviceFeatures));

    if (devfeat == NULL) {
      // TODO error
    }

    vkGetPhysicalDeviceFeatures(physical_devices[i], devfeat);

    VkPhysicalDevice *vk_physical_device =
      malloc(sizeof(VkPhysicalDevice));

    if (vk_physical_device == NULL) {
      // TODO error
    }

    *vk_physical_device = physical_devices[i];

    gfks_devices[i]->_protected->vk_physical_device = vk_physical_device;
    gfks_devices[i]->_protected->vk_physical_device_properties = devprops;
    gfks_devices[i]->_protected->vk_physical_device_features = devfeat;

    return gfks_devices;
}

// Adds information about available queue families to our gfks_device structs
void apply_queue_family_info_to_devices(gfks_device **gfks_devices,
                                        int device_count) {

  for (int i = 0; i < device_count; i++) {
    VkPhysicalDevice *physical_device =
      gfks_devices[i]->_protected->vk_physical_device;

    uint32_t queue_family_count = 0;                                                         
    vkGetPhysicalDeviceQueueFamilyProperties(*physical_device,
                                             &queue_family_count,
                                             NULL);

#if GFKS_DEBUG_LEVEL > 0
    printf("%s:Found %i queue families\n", GFKS_DEBUG_TAG, queue_family_count); 
#endif

    VkQueueFamilyProperties *queue_families = 
      malloc(sizeof(VkQueueFamilyProperties)*queue_family_count);

    if (queue_families == NULL) {
      // TODO error
    }

    vkGetPhysicalDeviceQueueFamilyProperties(*physical_device,
                                             &queue_family_count,
                                             queue_families);

    gfks_devices[i]->_protected->vk_queue_family_property_count =
      queue_family_count;

    gfks_devices[i]->_protected->vk_queue_family_properties =
      queue_families;
  }
}

#if GFKS_DEBUG_LEVEL > 0
  for (int i = 0; i < physical_device_count; i++) {
    VkPhysicalDeviceProperties devprops; 
    vkGetPhysicalDeviceProperties(physical_devices[i],&devprops);
    printf("  %s:Device %i is a %s\n",GFKS_DEBUG_TAG,i,devprops.deviceName);
  }
#endif
}

// Returns true if the device is suitable for drawing to the provided surface,
// otherwise false
static bool device_suitable(gfks_device *device, gfks_surface *surface) {
     
}

gfks_device** gfks_get_devices_suitable_for_surface(gfks_surface *surface) {
  int device_count;
  gfks_device **gfks_devices = 
    make_devs_from_physical_dev_enumeration(*(context->_protected->vk_instance),
                                            &device_count);

  apply_queue_family_info_to_devices(gfks_devices, device_count);
   
  
}
