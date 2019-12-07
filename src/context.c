#include "graffiks/internal.h"
#include <string.h>

// window system extension mapping
typedef struct {
  gfks_window_system window_system;
  char *vulkan_extension;
} window_system_extension_map;

static const window_system_extension_map window_system_extension_mapping[] = {
  {GFKS_WINDOW_SYSTEM_X11_BITFLAG,"VK_KHR_xlib_surface"},
  {GFKS_WINDOW_SYSTEM_WAYLAND_BITFLAG,"VK_KHR_wayland_surface"}
};

static const int window_system_extension_mapping_count =
sizeof(window_system_extension_mapping)/sizeof(window_system_extension_map);

// required engine extensions
static const char *engine_required_extensions[] = {"VK_KHR_surface"};
static const int engine_required_extension_count = 
sizeof(engine_required_extensions)/sizeof(*engine_required_extensions);

// define this function so wo can stick it at bottom of file
static const char** decide_extensions(gfks_window_system *window_systems,
                                      int *extension_count);

// Destroys a Graffiks context
static void gfks_free_context(gfks_context *context) {
  vkDestroyInstance(*(context->_protected->vk_instance), NULL);
  free(context->_protected->vk_instance);
  free(context->_protected->enabled_extensions);
  free(context->_protected);
  free(context);
}

// Creates a new Graffiks context
gfks_context* gfks_create_context(gfks_window_system *window_systems) {
#if (GFKS_DEBUG_LEVEL > 0)
  printf("%s: Creating context\n",GFKS_DEBUG_TAG);
#endif

  // allocate structs
  gfks_context *context = malloc(sizeof(gfks_context));
  context->_protected = malloc(sizeof(gfks_context_protected));

  if (context == NULL || context->_protected == NULL) {
    // TODO memory allocation error, give error before returning
    return NULL;
  }

  // define context function pointers
  context->free = &gfks_free_context;

  // put together app info struct
  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = NULL;
  app_info.applicationVersion = VK_MAKE_VERSION(0,0,0);
  app_info.pEngineName = GFKS_ENGINE_NAME;
  app_info.engineVersion = VK_MAKE_VERSION(GFKS_MAJOR_VERSION,GFKS_MINOR_VERSION,GFKS_REVISION);
  app_info.apiVersion = VK_API_VERSION_1_1;

  // decide what extensions we want to enable.
  context->_protected->enabled_extensions = decide_extensions(window_systems,
                                                  &(context->_protected->enabled_vulkan_extension_count));

  VkInstanceCreateInfo create_info = {};                               
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.enabledLayerCount = 0;
  create_info.enabledExtensionCount = context->_protected->enabled_vulkan_extension_count;
  create_info.ppEnabledExtensionNames = context->_protected->enabled_extensions;

#if (GFKS_DEBUG_LEVEL > 1)
  const char *validation_layers[1] = {"VK_LAYER_KHRONOS_validation"};
  // TODO check if validation layers exist first
  create_info.enabledLayerCount = 1;
  create_info.ppEnabledLayerNames = validation_layers;
#endif

  context->_protected->vk_instance = malloc(sizeof(VkInstance));
  VkResult r = vkCreateInstance(&create_info, NULL, context->_protected->vk_instance);
  if (r != VK_SUCCESS) {
    gfks_free_context(context);
    // TODO give user some type of error
    #if (GFKS_DEBUG_LEVEL > 0)
    printf("%s: Failed to create vulkan instance. gfks_context will be NULL. VkResult:%i\n",
           GFKS_DEBUG_TAG,r);
    #endif
    return NULL;
 }

  return context;
}


// Decides which vulkan extensions we want to enable
//
// - *extension_count will get written to the number of extensions returned.
// - returned array has extension_count rows and VK_MAX_EXTENSION_NAME_SIZE columns 
// - if there is no window system mapping available for a window system, or the
// extension is not available, the window system will be replaced with
// GFKS_WINDOW_SYSTEM_NONE_BITFLAG
static const char** decide_extensions(gfks_window_system *window_systems, int *extension_count) {
  // Define a linked list to store our decided-upon extensions
  struct extension_node {
    char *extension;
    struct extension_node *next;
  };

  // Figure out what extensions are available to us via the vulkan api
  uint32_t vulkan_supported_extension_count;
  vkEnumerateInstanceExtensionProperties(NULL, &vulkan_supported_extension_count, NULL);
  VkExtensionProperties vulkan_extension_properties[vulkan_supported_extension_count];
  vkEnumerateInstanceExtensionProperties(NULL,
                                         &vulkan_supported_extension_count,
                                         vulkan_extension_properties);

  #if (GFKS_DEBUG_LEVEL > 1)
  for (int i = 0; i < vulkan_supported_extension_count; i++) {
    printf("%s: Found supported vulkan extension: %s\n",
           GFKS_DEBUG_TAG,vulkan_extension_properties[i].extensionName);
  }
  #endif

  // Decide what window system extensions to enable and add them to the linked list
  struct extension_node *previous_node = NULL;
  struct extension_node *first_node = NULL;
  int total_extensions = 0;
  for (int i = 0; i < window_system_extension_mapping_count; i++)  {

    // If this window system mapping applies to any of our desired window systems...
    if (*window_systems & window_system_extension_mapping[i].window_system) {
      // Check if this extension is available 
      int extension_available = 0;
      for (int j = 0; j < vulkan_supported_extension_count; j++) {
        if (strcmp(window_system_extension_mapping[i].vulkan_extension,
                   vulkan_extension_properties[j].extensionName) == 0) {
          extension_available = 1;
        }
      }

      if (extension_available) {
        // Yup, we can enable this one. Add the extension to our linked list
        total_extensions++;

        struct extension_node *new_node = malloc(sizeof(struct extension_node));
        new_node->extension = window_system_extension_mapping[i].vulkan_extension;
        new_node->next = NULL;

        if (previous_node != NULL) {
          previous_node->next = new_node;
        } else {
          first_node = new_node;
        }

        previous_node = new_node;
      }
    }
  }

  #if (GFKS_DEBUG_LEVEL > 0)
  struct extension_node *current_node = first_node;
  while(current_node != NULL) {
    printf("%s: Decided to use instance extension: %s\n",GFKS_DEBUG_TAG,current_node->extension) ;
    current_node = current_node->next;
  }
  #endif

  // Add our required extensions to target array size
  total_extensions += engine_required_extension_count;

  // Allocate output extensions array
  const char **extensions = malloc((total_extensions*sizeof(char *))
                                   +(VK_MAX_EXTENSION_NAME_SIZE*sizeof(char)));

  char *offset = (char*)&extensions[total_extensions];
  for(int i = 0; i < total_extensions; i++) {
    extensions[i] = offset + i * VK_MAX_EXTENSION_NAME_SIZE;
  }

  // Write our linked list to our output array while freeing our linked list
  struct extension_node *cnode = first_node;
  struct extension_node *pnode = NULL;
  int i = 0;
  while(cnode != NULL) {
    extensions[i] = cnode->extension;
    cnode = cnode->next;

    if (pnode != NULL) {
      free(pnode);
    }

    pnode = cnode;
    i++;
  }
  free(cnode);

  // Add our required engine extensions
  for (int j = 0; j < engine_required_extension_count; j++)  {
      extensions[i++] = (char*)engine_required_extensions[j];
      #if (GFKS_DEBUG_LEVEL > 0)
        printf("%s: Decided to use instance extension: %s\n",
               GFKS_DEBUG_TAG,engine_required_extensions[j]) ;
      #endif
  }

  // TODO update window_systems

  *extension_count = total_extensions;
  return extensions;
}

