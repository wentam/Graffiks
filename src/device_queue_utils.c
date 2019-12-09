#include "graffiks/internal.h"

// Flags to represent the purpose of a queue
typedef enum {
  QUEUE_PURPOSE_GRAPHICS_BITFLAG=1,
  QUEUE_PURPOSE_COMPUTE_BITFLAG=2,
  QUEUE_PURPOSE_TRANSFER_BITFLAG=4,
  QUEUE_PURPOSE_SPARSE_BINDING_BITFLAG=8,
  QUEUE_PURPOSE_PRESENTATION_BITFLAG=16
} queue_purpose;

// Represents a decided-upon set of queues to create for a queue family.
typedef struct {
  // Creation info for the queue family
  VkDeviceQueueCreateInfo family_queue_create_info;

  // The number of queues we would like to create in this queue family 
  unsigned int queue_count;

  // An array with each value representing a queue in the family, whose values
  // describe what purposes we have decided this queue family should be used for
  queue_purpose *queue_purposes;
} queue_family_decision;

// Outputs information about our device queue families
static void print_device_queue_info(gfks_device *device) {
  int count = device->_protected->vk_queue_family_property_count;

  printf("%s: Found %i queue families for device %i\n",
         GFKS_DEBUG_TAG,
         count,
         device->_protected->vk_physical_device_index);

  for (int i = 0; i < count; i++) {
    printf("%s: - Queue family %i has count %i and flags 0x%08x\n",
           GFKS_DEBUG_TAG,
           i,
           device->_protected->vk_queue_family_properties->queueCount,
           device->_protected->vk_queue_family_properties->queueFlags);
  }
}

// Adds information about available queue families to the passed gfks_device
//
// Returns false on error, otherwise true
//
// If false is returned, we've already produced an error for the user of the
// engine
static bool
apply_queue_family_info_to_device(gfks_device *device) {
  // Grab physical device from our gfks_device
  VkPhysicalDevice physical_device =
    device->_protected->vk_physical_device;

  // Get number of queue families
  uint32_t queue_family_count = 0;                                                         
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device,
                                           &queue_family_count,
                                           NULL);

  // Allocate array of queue family properties
  VkQueueFamilyProperties *queue_families = 
    malloc(sizeof(VkQueueFamilyProperties)*queue_family_count);

  // Error on bad allocation
  if (gfks_check_alloc_throw_err(queue_families,__FILE__,__LINE__)) return false;

  // Fill our queue family property array
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device,
                                           &queue_family_count,
                                           queue_families);

  // Assign our info to the device
  device->_protected->vk_queue_family_property_count =
    queue_family_count;

  device->_protected->vk_queue_family_properties =
    queue_families;

#if GFKS_DEBUG_LEVEL > 0
  print_device_queue_info(device);
#endif

  return true;
}

// Frees the passed queue family decisions
static void free_queue_family_decisions(queue_family_decision *decisions,
                                        uint32_t decision_count) {
  for (int i = 0; i < decision_count; i++) {
    free(decisions[i].queue_purposes);
  }
  free(decisions);
}

// Decides how many queues to create in each queue family, and what the purpose
// of each queue should be.
//
// Passed surfaces are used to create presentation queues for each surface
//
// Returns an array of queue_family_decision, with each element representing a 
// queue family, or NULL if there was an error
//
// If NULL is returned, we've already produced an error for the user of the
// engine
//
// Free me with free_queue_family_decisions() when done!
static queue_family_decision
*decide_queues(gfks_device *device,
               gfks_surface *surfaces,
               unsigned int surface_count,
               unsigned int *queue_family_count_out) {

  unsigned int queue_family_count =
    device->_protected->vk_queue_family_property_count;

  *queue_family_count_out = queue_family_count;

  unsigned int graphics_queues_wanted = 1;

  // each index represents a passed surface.
  // if the value is true, we have yet to find a present queue for this surface
  // if the value is false, we have not found a present queue for this surface
  bool presentation_queues_wanted[surface_count];
  for (int i = 0; i < surface_count; i++) {
    presentation_queues_wanted[i] = true;
  }

  // Allocate decision info array 
  queue_family_decision *decisions =
    malloc(sizeof(queue_family_decision)*queue_family_count);

  if (gfks_check_alloc_throw_err(decisions,__FILE__,__LINE__)) return NULL;

  // Iterate over queue families and create one VkDeviceQueueCreateInfo object
  // for each
  for (int i = 0; i < queue_family_count; i++) {
    VkQueueFamilyProperties props =
      device->_protected->vk_queue_family_properties[i];

    int queues_available_in_family = props.queueCount;
    int queues_to_create_in_family = 0;
    VkQueueFlags *queue_decision_flags =
      malloc(sizeof(VkQueueFlags)*queues_available_in_family);

    if (gfks_check_alloc_throw_err(queue_decision_flags,__FILE__,__LINE__)) return NULL;

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

    for (int j = 0; j < surface_count; j++) {
      VkBool32 presentSupport = False;                                                          
      vkGetPhysicalDeviceSurfaceSupportKHR(device->_protected->vk_physical_device,
                                           i,
                                           surfaces[j]._protected->vk_surface,
                                           &presentSupport);

    // If this queue family supports presentation to our surface, and we still
    // want a presentation queue for this surface
      if (presentation_queues_wanted[j] == true &&
          (presentSupport == True)) {

        // Figure out how many queues we want from this queue family
        int queues_to_add = 1;
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
        presentation_queues_wanted[j] = false;
      }
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

// Creates queues in the provided gfks_device with the provided queue creation
// decisions.
//
// Returns true on success, false on failure
//
// If false is returned, we've already produced an error for the user of the
// engine
static bool create_queues(gfks_device *device,
                          queue_family_decision *queue_family_decisions,
                          int queue_family_count) {

  device->_protected->graphics_queue_count = 0;
  device->_protected->compute_queue_count = 0;
  device->_protected->transfer_queue_count = 0;
  device->_protected->sparse_binding_queue_count = 0;
  device->_protected->presentation_queue_count = 0;

  // Count up each type of queue we will be creating
  int total_queue_count = 0;
  for (int i = 0; i < queue_family_count; i++) {
    queue_family_decision d = queue_family_decisions[i];
    total_queue_count += d.queue_count;

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

  // Allocate our VkQueue array and associated data
  device->_protected->queues = malloc(sizeof(VkQueue)*total_queue_count);
  device->_protected->queue_count = total_queue_count;
  device->_protected->queue_families_for_queues =
    malloc(sizeof(int)*total_queue_count);

  if (gfks_check_alloc_throw_err(device->_protected->queues,__FILE__,__LINE__)) return NULL;
  if (gfks_check_alloc_throw_err(device->_protected->queue_families_for_queues,__FILE__,__LINE__)) return NULL;

  // Allocate our indices arrays 
  if (device->_protected->graphics_queue_count > 0) {
    device->_protected->graphics_queue_indices =
      malloc(sizeof(int)*device->_protected->graphics_queue_count);

    if (gfks_check_alloc_throw_err(device->_protected->graphics_queue_indices,__FILE__,__LINE__)) return NULL;
  }

  if (device->_protected->compute_queue_count > 0) {
    device->_protected->compute_queue_indices =
      malloc(sizeof(int)*device->_protected->compute_queue_count);
    if (gfks_check_alloc_throw_err(device->_protected->compute_queue_indices,__FILE__,__LINE__)) return NULL;
  }

  if (device->_protected->transfer_queue_count > 0) {
    device->_protected->transfer_queue_indices =
      malloc(sizeof(int)*device->_protected->transfer_queue_count);
    if (gfks_check_alloc_throw_err(device->_protected->transfer_queue_indices,__FILE__,__LINE__)) return NULL;
  }

  if (device->_protected->sparse_binding_queue_count > 0) {
    device->_protected->sparse_binding_queue_indices =
      malloc(sizeof(int)*device->_protected->sparse_binding_queue_count);
    if (gfks_check_alloc_throw_err(device->_protected->sparse_binding_queue_indices,__FILE__,__LINE__)) return NULL;
  }

  if (device->_protected->presentation_queue_count > 0) {
    device->_protected->presentation_queue_indices =
      malloc(sizeof(int)*device->_protected->presentation_queue_count);
    if (gfks_check_alloc_throw_err(device->_protected->presentation_queue_indices,__FILE__,__LINE__)) return NULL;
  }


  // write our queues
  int written_graphics_queues = 0;
  int written_compute_queues = 0;
  int written_transfer_queues = 0;
  int written_sparse_binding_queues = 0;
  int written_presentation_queues = 0;
  int total_written_queues = 0;
  for (int i = 0; i < queue_family_count; i++) {
    int used_queues_in_family = 0;
    queue_family_decision d = queue_family_decisions[i];
    for (int j = 0; j < d.queue_count; j++) {
      queue_purpose p = d.queue_purposes[i];

      VkQueue q;
      vkGetDeviceQueue(device->_protected->vk_logical_device,
                       i,
                       j,
                       &q);

      device->_protected->queues[total_written_queues] = q;
      device->_protected->queue_families_for_queues[total_written_queues] = i;

      if (p & QUEUE_PURPOSE_GRAPHICS_BITFLAG) {
        device->_protected->
          graphics_queue_indices[written_graphics_queues++] =
          total_written_queues;
      }

      if (p & QUEUE_PURPOSE_COMPUTE_BITFLAG) {
        device->_protected->
          compute_queue_indices[written_compute_queues++] =
          total_written_queues;
      }

      if (p & QUEUE_PURPOSE_TRANSFER_BITFLAG) {
        device->_protected->
          transfer_queue_indices[written_transfer_queues++] =
          total_written_queues;
      }

      if (p & QUEUE_PURPOSE_SPARSE_BINDING_BITFLAG) {
        device->_protected->
          sparse_binding_queue_indices[written_sparse_binding_queues++] =
          total_written_queues;
      }

      if (p & QUEUE_PURPOSE_PRESENTATION_BITFLAG) {
        device->_protected->
          presentation_queue_indices[written_presentation_queues++] =
          total_written_queues;
      }

      total_written_queues++;
    }
  }
}
