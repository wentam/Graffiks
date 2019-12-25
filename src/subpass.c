#include "graffiks/internal.h"
#include "surface_utils.c"

// TODO free all mallocs in this file

static bool create_pipeline_for_draw_step(gfks_subpass* subpass,
                                          draw_step* draw_step,
                                          VkRenderPass vk_render_pass,
                                          uint32_t subpass_index,
                                          uint32_t output_render_pass_index,
                                          uint32_t output_draw_step_index) {
  // TODO statix hax
  VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {};
  vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_state_create_info.vertexBindingDescriptionCount = 0;
  vertex_input_state_create_info.pVertexBindingDescriptions = NULL;
  vertex_input_state_create_info.vertexAttributeDescriptionCount = 0;
  vertex_input_state_create_info.pVertexAttributeDescriptions = NULL;

  // TODO statix hax
  VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {};
  input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;


  // TODO static hax
  // set up color blending info
  VkPipelineColorBlendAttachmentState color_blend_attachment = {};
  color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                          VK_COLOR_COMPONENT_G_BIT |
                                          VK_COLOR_COMPONENT_B_BIT |
                                          VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_FALSE;
  VkPipelineColorBlendStateCreateInfo color_blending = {};
  color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending.logicOpEnable = VK_FALSE;
  color_blending.attachmentCount = 1;
  color_blending.pAttachments = &color_blend_attachment;

  // TODO static hax
  // set up pipeline layout
  VkPipelineLayout pipeline_layout;
  VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
  pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

  if (vkCreatePipelineLayout(subpass->device->_protected->vk_logical_device,
                             &pipeline_layout_create_info,
                             NULL,
                             &pipeline_layout) != VK_SUCCESS) {
    printf("Failed to create pipeline layout\n");
  }

  // Create pipeline 
  VkPipelineShaderStageCreateInfo vk_shader_set[draw_step->shader_count];
  for (int i = 0; i < draw_step->shader_count; i++) {
    vk_shader_set[i] = draw_step->shader_set[i]->_protected->vk_shader_stage_create_info;
  }

  VkGraphicsPipelineCreateInfo pipeline_create_info = {};
  pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_create_info.stageCount = draw_step->shader_count;
  pipeline_create_info.pStages = vk_shader_set;
  pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;
  pipeline_create_info.pInputAssemblyState = &input_assembly_state_create_info;
  pipeline_create_info.pViewportState =
    &(subpass->_protected->viewport_state_create_info);

  pipeline_create_info.pRasterizationState = &(draw_step->rsettings->_protected->settings);
  pipeline_create_info.pMultisampleState = &(draw_step->msettings->_protected->settings);
  pipeline_create_info.pDepthStencilState = NULL;
  pipeline_create_info.pColorBlendState = &color_blending;
  pipeline_create_info.pDynamicState = NULL;
  pipeline_create_info.layout = pipeline_layout;
  pipeline_create_info.renderPass = vk_render_pass;
  pipeline_create_info.subpass = subpass_index;
  pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
  pipeline_create_info.basePipelineIndex = -1;

  VkPipeline pipeline;

  if (vkCreateGraphicsPipelines(subpass->device->_protected->vk_logical_device,
                                VK_NULL_HANDLE,
                                1,
                                &pipeline_create_info,
                                NULL,
                                &pipeline) != VK_SUCCESS) {
    // TODO error
  } 

  subpass->_protected->vk_pipelines[output_render_pass_index][output_draw_step_index] = pipeline;

  return true;
}

static uint32_t gfks_subpass_add_shader_set(gfks_subpass *subpass,
                                                uint32_t shader_count,
                                                gfks_shader **shader_set) {

  // TODO error on malloc NULL
  uint32_t* draw_step_count = &(subpass->_protected->draw_step_count);
  subpass->_protected->draw_steps[*draw_step_count] = malloc(sizeof(draw_step));

  // Grab an easy handle to our new draw step
  draw_step *draw_step = subpass->_protected->draw_steps[*draw_step_count];

  // Copy the shader pointers in the users shader set into our own set.
  // We need to own the memory.
  draw_step->shader_count = shader_count;
  draw_step->shader_set = malloc(sizeof(gfks_shader)*shader_count);
  for (int i = 0; i < shader_count; i++) {
    draw_step->shader_set[i] = shader_set[i];
  }

  // Define our rasterization/multisampling settings with the defaults
  draw_step->rsettings =
    subpass->context->_protected->defaults->rasterization_settings;
  draw_step->msettings =
    subpass->context->_protected->defaults->multisample_settings;

  return (*draw_step_count)++;
}

static int gfks_subpass_set_up_for_render_pass(gfks_subpass *subpass,
                                                gfks_render_pass *render_pass,
                                                VkRenderPass vk_render_pass,
                                                uint32_t subpass_index) {
 
  uint32_t* info_count = &(subpass->_protected->render_pass_info_count);
  
  subpass->_protected->render_pass_info[*info_count].render_pass = render_pass;
  subpass->_protected->render_pass_info[*info_count].vk_render_pass = vk_render_pass;
  subpass->_protected->render_pass_info[*info_count].subpass_index = subpass_index;

  // Create vulkan pipelines for every draw step that already exists
  // TODO: create them also when new shaders are added

  for (int i = 0; i < subpass->_protected->draw_step_count; i++) {
    if (!create_pipeline_for_draw_step(subpass,
                                       subpass->_protected->draw_steps[i],
                                       vk_render_pass,
                                       subpass_index,
                                       *info_count,
                                       i)) return -1;    
  }

  return (*info_count)++;
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
  new_pass->_protected->render_pass_info_count = 0;

  // Assign NULL to all pointer members
  new_pass->context = NULL;
  new_pass->device = NULL;

  // Assign public cmethod pointers
  new_pass->add_shader_set = &gfks_subpass_add_shader_set;
  new_pass->set_shaderset_rasterization = &gfks_subpass_set_shaderset_rasterization;
  new_pass->set_shaderset_multisampling = &gfks_subpass_set_shaderset_multisampling;

  // Assign private method pointers
  new_pass->_protected->set_up_for_render_pass = &gfks_subpass_set_up_for_render_pass;

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
