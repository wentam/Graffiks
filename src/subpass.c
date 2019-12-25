#include "graffiks/internal.h"
#include "surface_utils.c"

// TODO free all mallocs in this file


static uint32_t gfks_subpass_add_shader_set(gfks_subpass *subpass,
                                                uint32_t shader_count,
                                                gfks_shader **shader_set) {

  // TODO error on malloc NULL
  uint32_t* draw_step_count = &(subpass->_protected->draw_step_count);
  subpass->_protected->draw_steps[*draw_step_count] = malloc(sizeof(draw_step));

  // Copy the shader pointers in the users shader set into our own set.
  // We need to own the memory.
  subpass->_protected->draw_steps[*draw_step_count]->shader_count = shader_count;
  subpass->_protected->draw_steps[*draw_step_count]->shader_set = malloc(sizeof(gfks_shader)*shader_count);
  for (int i = 0; i < shader_count; i++) {
    subpass->_protected->draw_steps[*draw_step_count]->shader_set[i] = shader_set[i];
  }

  // Define our rasterization/multisampling settings with the defaults
  subpass->_protected->draw_steps[*draw_step_count]->rsettings =
    subpass->context->_protected->defaults->rasterization_settings;
  subpass->_protected->draw_steps[*draw_step_count]->msettings =
    subpass->context->_protected->defaults->multisample_settings;

  return (*draw_step_count)++;
}

static void gfks_subpass_set_shaderset_rasterization(gfks_subpass *subpass,
                                                         uint32_t shaderset_index,
                                                         gfks_rasterization_settings *settings) {
  
  subpass->_protected->draw_steps[shaderset_index]->rsettings = settings;
}

static void gfks_subpass_set_shaderset_multisampling(gfks_subpass *subpass,
                                                        uint32_t shaderset_index,
                                                        gfks_multisample_settings *settings) {
  
  subpass->_protected->draw_steps[shaderset_index]->msettings = settings;
}

static gfks_subpass* init_struct() {
  // Allocate memory for our struct
  gfks_subpass* new_pass = malloc(sizeof(gfks_subpass));
  new_pass->_protected = malloc(sizeof(gfks_subpass_protected));

  // Error and return NULL on failed allocation
  if (new_pass == NULL || new_pass->_protected == NULL) {
    gfks_err(GFKS_ERROR_FAILED_MEMORY_ALLOCATION, 1, "Failed to allocate memory");
    return NULL;
  }

  // Initialize primitive members
  new_pass->_protected->draw_step_count = 0;

  // Assign NULL to all pointer members
  new_pass->context = NULL;
  new_pass->device = NULL;

  // Assign method pointers
  new_pass->add_shader_set = &gfks_subpass_add_shader_set;
  new_pass->set_shaderset_rasterization = &gfks_subpass_set_shaderset_rasterization;
  new_pass->set_shaderset_multisampling = &gfks_subpass_set_shaderset_multisampling;

  // Return the struct
  return new_pass;
}

static void gfks_free_subpass(gfks_subpass *subpass) {
  free(subpass->_protected);
  free(subpass);
}

gfks_subpass* gfks_create_subpass(gfks_context *context,
                                          gfks_device *device,
                                          float width,
                                          float height) {
  // Init our struct
  gfks_subpass* new_pass = init_struct();
  new_pass->context = context;
  new_pass->device = device;

  // Define our viewport and scissor
  // TODO: This viewport/scissor information is used only for pipeline creation,
  // so it would make sense that the user would specify it for each shader set.
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
  // TODO allocate this memory dynamically, growing as needed.
  new_pass->_protected->draw_steps = malloc(sizeof(draw_step *) * 256);

  return new_pass;
}