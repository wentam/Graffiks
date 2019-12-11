#include "graffiks/internal.h"
#include "surface_utils.c"

// TODO free all mallocs in this file

static bool set_up_presentation_surface(gfks_render_pass *render_pass,
                                        uint8_t surface_index) {

   // Grab our surface and device
   gfks_surface *surface = render_pass->_protected->presentation_surfaces[surface_index];
   gfks_device *device = render_pass->device;

   // Allocate memory for our presentation surface data and get an easy handle to it
   render_pass->_protected->presentation_surface_data[surface_index] =
     malloc(sizeof(presentation_surface_data));
   presentation_surface_data *psd = render_pass->_protected->presentation_surface_data[surface_index];

   // Define our surface capabilities
   if (!gfks_surface_util_obtain_surface_capabilities_for_device(surface,
                                                                 device,
                                                                 &(psd->surface_capabilities))) {
     return false;
   }

   // Decide swap chain image count
   uint32_t desired_swap_chain_image_count =
     gfks_surface_util_decide_desired_swap_chain_image_count(psd->surface_capabilities);

   // Decide swap chain image size
   psd->swap_chain_extent =
     gfks_surface_util_decide_swap_chain_image_size(psd->surface_capabilities, surface);

   // Decide surface format
   if (!gfks_surface_util_decide_surface_format_for_device(surface,
                                                           device,
                                                           &(psd->surface_format))) return false;

   // Decide presentation mode
   if (!gfks_surface_util_decide_surface_presentation_mode_for_device(surface,
                                                    device,
                                                    &(psd->surface_presentation_mode))) return false;

   // Create our swap chain
   if(!gfks_surface_util_create_swap_chain(surface,
                                           device,
                                           psd->surface_capabilities,
                                           psd->surface_format,
                                           psd->surface_presentation_mode,
                                           psd->swap_chain_extent,
                                           &(psd->swap_chain),
                                           &(psd->swap_chain_image_count),
                                           &(psd->swap_chain_images),
                                           &(psd->swap_chain_image_views),
                                           desired_swap_chain_image_count)) return false;

   return true;
}

static int16_t gfks_render_pass_add_presentation_surface(gfks_render_pass *render_pass,
                                                        gfks_surface *surface) {

  // get our surface count
  uint8_t pcount = render_pass->_protected->presentation_surface_count;

  // See if there's an existing NULL spot we can use
  int16_t chosen_index = -1;
  for (int i = 0; i < pcount; i++) {
    if (render_pass->_protected->presentation_surfaces[i] == NULL) {
      chosen_index = i;
      break;
    }
  }

  // If we didn't find an empty spot, we'll add our surface to the end of the array
  if (chosen_index == -1) chosen_index = pcount;

  // Error if we have too many presentation surfaces already
  if (chosen_index >= GFKS_MAX_RENDER_PASS_PRESENTATION_SURFACES) {
    // TODO error
  }

  // Assign our surface
  render_pass->_protected->presentation_surfaces[chosen_index] = surface;

  // If we were added to the end, increment our count
  if (chosen_index == pcount) render_pass->_protected->presentation_surface_count++;

  // Set up our presentation surface
  if (!set_up_presentation_surface(render_pass,
                                   chosen_index)) return -1;
}

static bool gfks_render_pass_remove_presentation_surface(gfks_render_pass *render_pass,
                                                         uint8_t index) {
  render_pass->_protected->presentation_surfaces[index] = NULL;

  if (index == (render_pass->_protected->presentation_surface_count)-1) {
    render_pass->_protected->presentation_surface_count--;
  }

 // TODO free our presentation surface data, and destroy vulkan objects within
}

static gfks_render_pass* init_struct() {
  // Allocate memory for our struct
  gfks_render_pass* new_pass = malloc(sizeof(gfks_render_pass));
  new_pass->_protected = malloc(sizeof(gfks_render_pass_protected));

  // Error and return NULL on failed allocation
  if (new_pass == NULL || new_pass->_protected == NULL) {
    gfks_err(GFKS_ERROR_FAILED_MEMORY_ALLOCATION, 1, "Failed to allocate memory");
    return NULL;
  }

  // Initialize primitive members
  new_pass->_protected->presentation_surface_count = 0;

  // Assign NULL to all pointer members
  new_pass->context = NULL;
  new_pass->device = NULL;
  new_pass->_protected->presentation_surfaces = NULL;
  new_pass->_protected->presentation_surface_data = NULL;

  // Assign method pointers
  new_pass->add_presentation_surface = &gfks_render_pass_add_presentation_surface;
  new_pass->remove_presentation_surface = &gfks_render_pass_remove_presentation_surface;

  // Return the struct
  return new_pass;
}

static void gfks_free_render_pass(gfks_render_pass *render_pass) {
  free(render_pass->_protected);
  free(render_pass);
}

gfks_render_pass* gfks_create_render_pass(gfks_context *context, gfks_device *device, float width, float height) {
  // Init our struct
  gfks_render_pass* new_pass = init_struct();
  new_pass->context = context;
  new_pass->device = device;

  // Define our viewport and scissor
  // TODO: This viewport/scissor information is used only for pipeline creation,
  // so it would make sense that the user would specify it for each material/shader chain.
  // it also looks like it's possible to add multiple viewports. no idea what that does, but
  // we probably want to be able to utilize it
  VkViewport viewport = {};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = width;
  viewport.height = height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkExtent2D e = {};
  e.width = width;
  e.height = height;

  VkRect2D scissor = {};
  scissor.offset = (VkOffset2D){0.0f, 0.0f};
  scissor.extent = e;

  new_pass->_protected->viewport = viewport;
  new_pass->_protected->scissor = scissor;

  VkPipelineViewportStateCreateInfo view_port_state_create_info = {};
  view_port_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  view_port_state_create_info.viewportCount = 1;
  view_port_state_create_info.pViewports = &(new_pass->_protected->viewport);
  view_port_state_create_info.scissorCount = 1;
  view_port_state_create_info.pScissors = &(new_pass->_protected->scissor);


  new_pass->_protected->viewport_state_create_info = view_port_state_create_info;

  // Allocate memory for our presentation surfaces
  // TODO it's probably best to allocate this memory dynamically, growing as needed.
  new_pass->_protected->presentation_surfaces = malloc(sizeof(gfks_surface *) *
                                                       GFKS_MAX_RENDER_PASS_PRESENTATION_SURFACES);
  new_pass->_protected->presentation_surface_data = malloc(sizeof(presentation_surface_data *) *
                                                           GFKS_MAX_RENDER_PASS_PRESENTATION_SURFACES);

  return new_pass;
}
