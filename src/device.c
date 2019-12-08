#include "graffiks/internal.h"

// Allocates our gfks_device structs and fills them with information about
// the devices
static gfks_device*
create_devices(VkInstance vk_instance,
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
  gfks_device *gfks_devices = 
    malloc(sizeof(gfks_device)*physical_device_count);

  if (gfks_devices == NULL) {
    // TODO error
  }

  // Assign device properties to our devices
  for (int i = 0; i < physical_device_count; i++) {
  
    // Set up our device struct
    gfks_devices[i]._protected = malloc(sizeof(gfks_device_protected));

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

#if GFKS_DEBUG_LEVEL > 0
    printf("%s: - Device %i is a %s\n",GFKS_DEBUG_TAG,i,devprops->deviceName);
#endif

    gfks_devices[i]._protected->vk_physical_device = physical_devices[i];
    gfks_devices[i]._protected->vk_physical_device_index = i;
    gfks_devices[i]._protected->vk_physical_device_properties = devprops;
    gfks_devices[i]._protected->vk_physical_device_features = devfeat;
  }

  return gfks_devices;
}

// Adds information about available queue families to the passed gfks_device
static void 
apply_queue_family_info_to_device(gfks_device *device) {

  VkPhysicalDevice physical_device =
    device->_protected->vk_physical_device;

  uint32_t queue_family_count = 0;                                                         
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device,
                                           &queue_family_count,
                                           NULL);

#if GFKS_DEBUG_LEVEL > 0
  printf("%s: Found %i queue families for device %i\n",
         GFKS_DEBUG_TAG,
         queue_family_count,
         device->_protected->vk_physical_device_index); 
#endif

  VkQueueFamilyProperties *queue_families = 
    malloc(sizeof(VkQueueFamilyProperties)*queue_family_count);

  if (queue_families == NULL) {
    // TODO error
  }

  vkGetPhysicalDeviceQueueFamilyProperties(physical_device,
                                           &queue_family_count,
                                           queue_families);

#if GFKS_DEBUG_LEVEL > 0
  for (int i = 0; i < queue_family_count; i++) {
    printf("%s: - Queue family %i has count %i and flags 0x%08x\n",
           GFKS_DEBUG_TAG,
           i,
           queue_families[i].queueCount,
           queue_families[i].queueFlags);
  }
#endif

  device->_protected->vk_queue_family_property_count =
    queue_family_count;

  device->_protected->vk_queue_family_properties =
    queue_families;
}

// Queue family questions to be figured out:
// * If available, is it better for the presentation queue to be the same queue
// as another?
// * Can using more than one queue increase total available bandwidth?
// * Is there ever a reason that it would be preferred to have a single queue
// that does two things, as opposed to a separate queue for each thing?
// (like graphics and transfers)
// * If there are multiple queue families that support, say, graphics,
// can one queue family be faster than another? How would I tell?
// TODO: * I remember reading that queues expire and need to be recreated.
// if this is the case, handle this!
//
// It sounds like the exact behavior of queue families is very
// implementation-specific, so queue selection could be challenging


// Decides what queues to create for the provided device and surface
// Returns array of VkDeviceQueueCreateInfo, with each element representing a
// queue family
//
// Free the result when you're done!

typedef enum {
  QUEUE_PURPOSE_GRAPHICS_BITFLAG=1,
  QUEUE_PURPOSE_COMPUTE_BITFLAG=2,
  QUEUE_PURPOSE_TRANSFER_BITFLAG=4,
  QUEUE_PURPOSE_SPARSE_BINDING_BITFLAG=8,
  QUEUE_PURPOSE_PRESENTATION_BITFLAG=16
} queue_purpose;

typedef struct {
  VkDeviceQueueCreateInfo family_queue_create_info;
  
  unsigned int queue_count;

  // An array with each value representing a queue in the family, whose values
  // describe what purposes we have decided this queue family should be used for
  queue_purpose *queue_purposes;
} queue_family_decision;

// Free me with free_queue_family_decisions() when done!
static queue_family_decision
*decide_queues(gfks_device *device,
               gfks_surface *surface,
               unsigned int *queue_family_count_out) {

  unsigned int queue_family_count =
    device->_protected->vk_queue_family_property_count;

  *queue_family_count_out = queue_family_count;

  unsigned int graphics_queues_wanted = 1;
  unsigned int presentation_queues_wanted = 1;

  // Allocate decision info array 
  queue_family_decision *decisions =
    malloc(sizeof(queue_family_decision)*queue_family_count);
 
  if (decisions == NULL) {
    // TODO error 
  }

  // Iterate over queue families and create one VkDeviceQueueCreateInfo object
  // for each
  for (int i = 0; i < queue_family_count; i++) {
    VkQueueFamilyProperties props = device->_protected->vk_queue_family_properties[i];

    int queues_available_in_family = props.queueCount;
    int queues_to_create_in_family = 0;
    VkQueueFlags *queue_decision_flags =
      malloc(sizeof(VkQueueFlags)*queues_available_in_family);

    *queue_decision_flags = 0;

    // If this queue family supports graphics, and we still want graphics queues
    if (graphics_queues_wanted > 0 &&
        (props.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {

      // Figure out how many queues we want from this queue family
      int queues_to_add = graphics_queues_wanted;
      if (queues_to_add > queues_available_in_family) {
        queues_to_add = queues_available_in_family;
      }
   
      // Make sure we create at least the number of queues that we want out of
      // this family
      if (queues_to_create_in_family < queues_to_add) {
        queues_to_create_in_family = queues_to_add;
      }

      // Mark the first queues_to_add queues in our decision flags as graphics
      // queues (we've chosen potentially overlapping queues).
      for (int j = 0; j < queues_to_add; j++) {
        queue_decision_flags[j] |= QUEUE_PURPOSE_GRAPHICS_BITFLAG;
      }

      // We found our queues - remove from wanted count
      graphics_queues_wanted -= queues_to_add;
    }

    // If this queue family supports presentation to our surface, and we still
    // want presentation queues
    VkBool32 presentSupport = False;                                                          
    vkGetPhysicalDeviceSurfaceSupportKHR(device->_protected->vk_physical_device, i, surface->_protected->vk_surface, &presentSupport);
    if (presentation_queues_wanted > 0 &&
        (presentSupport == True)) {
     
      // Figure out how many queues we want from this queue family
      int queues_to_add = presentation_queues_wanted;
      if (queues_to_add > queues_available_in_family) {
        queues_to_add = queues_available_in_family;
      }

      // Make sure we create at least the number of queues that we want out of
      // this family
      if (queues_to_create_in_family < queues_to_add) {
        queues_to_create_in_family = queues_to_add;
      }

      // Mark the first queues_to_add queues in our decision flags as
      // presentation queues (we've chosen potentially overlapping queues).
      for (int j = 0; j < queues_to_add; j++) {
        queue_decision_flags[j] |= QUEUE_PURPOSE_PRESENTATION_BITFLAG;
      }

      // We found our queues - remove from wanted count
      presentation_queues_wanted -= queues_to_add;
    }

    decisions[i].queue_purposes = queue_decision_flags;
    if (queues_to_create_in_family > 0) {
      VkDeviceQueueCreateInfo create_info = {};
      create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      create_info.queueFamilyIndex = i;
      create_info.queueCount = queues_to_create_in_family;

      float queuePriority = 1.0f;
      create_info.pQueuePriorities = &queuePriority;


      decisions[i].family_queue_create_info = create_info;
      decisions[i].queue_count = queues_to_create_in_family;


    } else {
      decisions[i].queue_count = 0;
    }
  }

  return decisions;
}


const char * const *decide_device_extensions(gfks_device *device, unsigned int *count) {
  // TODO check if our device extensions are available 
  int required_device_extension_count = 1;
  char **required_device_extension_names = malloc(sizeof(char *)*required_device_extension_count);
  required_device_extension_names[0] = malloc(sizeof(char)*VK_MAX_EXTENSION_NAME_SIZE);
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
 if (vkCreateDevice(device->_protected->vk_physical_device, &device_create_info, NULL, &ldevice) != VK_SUCCESS) {
   // TODO error
   return false;
 }
 device->_protected->vk_logical_device = ldevice; 

 return true;
}

void create_queues(gfks_device *device,
                   queue_family_decision *queue_family_decisions,
                   int queue_family_count) {

  device->_protected->graphics_queue_count = 0;
  device->_protected->compute_queue_count = 0;
  device->_protected->transfer_queue_count = 0;
  device->_protected->sparse_binding_queue_count = 0;
  device->_protected->presentation_queue_count = 0;

  // Count up each type of queue we will be creating
  for (int i = 0; i < queue_family_count; i++) {
    queue_family_decision d = queue_family_decisions[i];
    for (int j = 0; j < d.queue_count; j++) {
      queue_purpose p = d.queue_purposes[i];

      if (p & QUEUE_PURPOSE_GRAPHICS_BITFLAG) {
        device->_protected->graphics_queue_count++;
      }

      if (p & QUEUE_PURPOSE_COMPUTE_BITFLAG) {
        device->_protected->compute_queue_count++;
      }

      if (p & QUEUE_PURPOSE_TRANSFER_BITFLAG) {
        device->_protected->transfer_queue_count++;
      }

      if (p & QUEUE_PURPOSE_SPARSE_BINDING_BITFLAG) {
        device->_protected->sparse_binding_queue_count++;
      }

      if (p & QUEUE_PURPOSE_PRESENTATION_BITFLAG) {
        device->_protected->presentation_queue_count++;
      }
    }
  }

  // Allocate our VkQueue objects
  if (device->_protected->graphics_queue_count > 0) {
    device->_protected->graphics_queues = malloc(sizeof(VkQueue)*device->_protected->graphics_queue_count);
  }

  if (device->_protected->compute_queue_count > 0) {
    device->_protected->compute_queues = malloc(sizeof(VkQueue)*device->_protected->compute_queue_count);
  }

  if (device->_protected->transfer_queue_count > 0) {
    device->_protected->transfer_queues = malloc(sizeof(VkQueue)*device->_protected->transfer_queue_count);
  }

  if (device->_protected->sparse_binding_queue_count > 0) {
    device->_protected->sparse_binding_queues = malloc(sizeof(VkQueue)*device->_protected->sparse_binding_queue_count);
  }

  if (device->_protected->presentation_queue_count > 0) {
    device->_protected->presentation_queues = malloc(sizeof(VkQueue)*device->_protected->presentation_queue_count);
  }

  // write our queues
  int written_graphics_queues = 0;
  int written_compute_queues = 0;
  int written_transfer_queues = 0;
  int written_sparse_binding_queues = 0;
  int written_presentation_queues = 0;
  for (int i = 0; i < queue_family_count; i++) {
    int used_queues_in_family = 0;
    queue_family_decision d = queue_family_decisions[i];
    for (int j = 0; j < d.queue_count; j++) {
      queue_purpose p = d.queue_purposes[i];

      if (p & QUEUE_PURPOSE_GRAPHICS_BITFLAG) {
        VkQueue q;
        vkGetDeviceQueue(device->_protected->vk_logical_device,
                         i,
                         j,
                         &q);

        device->_protected->graphics_queues[written_graphics_queues++] = q;
      }

      if (p & QUEUE_PURPOSE_COMPUTE_BITFLAG) {
        VkQueue q;
        vkGetDeviceQueue(device->_protected->vk_logical_device,
                         i,
                         j,
                         &q);

        device->_protected->compute_queues[written_compute_queues++] = q;
      }

      if (p & QUEUE_PURPOSE_TRANSFER_BITFLAG) {
        VkQueue q;
        vkGetDeviceQueue(device->_protected->vk_logical_device,
                         i,
                         j,
                         &q);

        device->_protected->transfer_queues[written_transfer_queues++] = q;
      }

      if (p & QUEUE_PURPOSE_SPARSE_BINDING_BITFLAG) {
        VkQueue q;
        vkGetDeviceQueue(device->_protected->vk_logical_device,
                         i,
                         j,
                         &q);

        device->_protected->sparse_binding_queues[written_sparse_binding_queues++] = q;
      }

      if (p & QUEUE_PURPOSE_PRESENTATION_BITFLAG) {
        VkQueue q;
        vkGetDeviceQueue(device->_protected->vk_logical_device,
                         i,
                         j,
                         &q);

        device->_protected->presentation_queues[written_presentation_queues++] = q;
      }
    }
  }
}

// Outputs an array of devices suitable for rendering graphics and presenting
// to the provided surface
gfks_device*
gfks_get_devices_suitable_for_surface(gfks_surface *surface) {
#if GFKS_DEBUG_LEVEL > 1
  printf("%s: Getting devices suitable for surface.\n",GFKS_DEBUG_TAG);
#endif

  // Create our devices
  int device_count;

  gfks_device *gfks_devices = 
    create_devices(*(surface->context->_protected->vk_instance),
                   &device_count);

  if (gfks_devices == NULL) {
    // TODO error if no error was produced inside function
  }

  // Figure out what queues to create for each device, 
  // figure out what extensions to enable,
  // create our logical vulkan device,
  // and create queues for it
  for (int i = 0; i < device_count; i++) {
    // Get information about our queues
    apply_queue_family_info_to_device(&(gfks_devices[i])); 

    // Decide what queues to create
    unsigned int queue_family_count;

    queue_family_decision *queue_family_decisions =
      decide_queues(&(gfks_devices[i]), surface, &queue_family_count);

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
      decide_device_extensions(&(gfks_devices[i]), &device_extension_count);

    if (create_logical_device(&(gfks_devices[i]),
                                extension_decisions,
                                device_extension_count,
                                queue_family_decisions,
                                queue_family_count) == false) {
      // TODO error
    }

    create_queues(&(gfks_devices[i]),queue_family_decisions, queue_family_count); 
  }

  // TODO test devices for suitability
  return gfks_devices;
}
