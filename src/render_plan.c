#include "graffiks/internal.h"

static void gfks_render_plan_add_render_pass(gfks_render_plan *plan, gfks_render_pass *pass) {
  uint32_t *pcount = &(plan->_protected->render_pass_count);
  plan->_protected->render_passes[*pcount] = pass;
  (*pcount)++;
}

static bool gfks_render_plan_finalize_plan(gfks_render_plan *plan) {
  VkDevice vk_device = plan->device->_protected->vk_logical_device;

  // TODO static hax
  // define multisampling info
  // (we won't bother with multisampling for this test)
  VkPipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading = 1.0f;
  multisampling.pSampleMask = NULL;
  multisampling.alphaToCoverageEnable = VK_FALSE;
  multisampling.alphaToOneEnable = VK_FALSE;

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

  if (vkCreatePipelineLayout(vk_device, &pipeline_layout_create_info, NULL, &pipeline_layout) != VK_SUCCESS) {
    printf("Failed to create pipeline layout\n");
  }

  // TODO static hax
  // subpass dependencies
  VkSubpassDependency dep = {};
  dep.srcSubpass = VK_SUBPASS_EXTERNAL;
  dep.dstSubpass = 0;
  dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dep.srcAccessMask = 0;
  dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  // TODO static hax
  // define render pass
  VkAttachmentDescription color_attachment = {};
  color_attachment.format =
    plan->_protected->render_passes[0]->_protected->presentation_surface_data[0]->surface_format.format;
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_attachment_ref = {};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_ref;

  VkRenderPass render_pass;

  VkRenderPassCreateInfo render_pass_create_info = {};
  render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_create_info.attachmentCount = 1;
  render_pass_create_info.pAttachments = &color_attachment;
  render_pass_create_info.subpassCount = 1;
  render_pass_create_info.pSubpasses = &subpass;
  render_pass_create_info.dependencyCount = 1;
  render_pass_create_info.pDependencies = &dep;

  if (vkCreateRenderPass(vk_device, &render_pass_create_info, NULL, &render_pass) != VK_SUCCESS) {
    // TODO error
  }

  // TODO static hax
  // set up input info
  VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {};
  vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_state_create_info.vertexBindingDescriptionCount = 0;
  vertex_input_state_create_info.pVertexBindingDescriptions = NULL;
  vertex_input_state_create_info.vertexAttributeDescriptionCount = 0;
  vertex_input_state_create_info.pVertexAttributeDescriptions = NULL;

  VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {};
  input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;

  // TODO static hax
  // create pipeline
  uint32_t shader_count = plan->_protected->render_passes[0]->_protected->draw_steps[0]->shader_count;
  gfks_shader **shader_set = plan->_protected->render_passes[0]->_protected->draw_steps[0]->shader_set;

  VkPipelineShaderStageCreateInfo vk_shader_set[shader_count];
  for (int i = 0; i < shader_count; i++) {
    vk_shader_set[i] = shader_set[i]->_protected->vk_shader_stage_create_info;
  }

  VkGraphicsPipelineCreateInfo pipeline_create_info = {};
  pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_create_info.stageCount = 2;
  pipeline_create_info.pStages = vk_shader_set;
  pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;
  pipeline_create_info.pInputAssemblyState = &input_assembly_state_create_info;
  pipeline_create_info.pViewportState = &(plan->_protected->render_passes[0]->_protected->viewport_state_create_info);
  pipeline_create_info.pRasterizationState = &(plan->_protected->render_passes[0]->_protected->draw_steps[0]->rsettings->_protected->settings);
  pipeline_create_info.pMultisampleState = &multisampling;
  pipeline_create_info.pDepthStencilState = NULL;
  pipeline_create_info.pColorBlendState = &color_blending;
  pipeline_create_info.pDynamicState = NULL;
  pipeline_create_info.layout = pipeline_layout;
  pipeline_create_info.renderPass = render_pass;
  pipeline_create_info.subpass = 0;
  pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
  pipeline_create_info.basePipelineIndex = -1;

  VkPipeline graphics_pipeline;

  if (vkCreateGraphicsPipelines(vk_device,
                                VK_NULL_HANDLE,
                                1,
                                &pipeline_create_info,
                                NULL,
                                &graphics_pipeline) != VK_SUCCESS) {
    // TODO error
  }


  // TODO static hax
  // create a framebuffer for each swapchain image view
  uint32_t swap_chain_image_count =
    plan->_protected->render_passes[0]->_protected->presentation_surface_data[0]->swap_chain_image_count;

  VkImageView *swap_chain_image_views =
    plan->_protected->render_passes[0]->_protected->presentation_surface_data[0]->swap_chain_image_views;

  VkExtent2D swap_chain_extent =
    plan->_protected->render_passes[0]->_protected->presentation_surface_data[0]->swap_chain_extent;

  VkFramebuffer swap_chain_frame_buffers[swap_chain_image_count];

  for (int i = 0; i < swap_chain_image_count; i++) {
    VkImageView attachments[] = {swap_chain_image_views[i]};

    VkFramebufferCreateInfo frame_buffer_create_info = {};
    frame_buffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frame_buffer_create_info.renderPass = render_pass;
    frame_buffer_create_info.attachmentCount =1;
    frame_buffer_create_info.pAttachments  = attachments;
    frame_buffer_create_info.width = swap_chain_extent.width;
    frame_buffer_create_info.height = swap_chain_extent.height;
    frame_buffer_create_info.layers = 1;

    if (vkCreateFramebuffer(vk_device, &frame_buffer_create_info, NULL, &swap_chain_frame_buffers[i]) != VK_SUCCESS) {
      printf("Failed to create framebuffer for swap buffer image %i\n", i);
      exit(0);
    }
  }

  // TODO static hax
  // create command pool
  VkCommandPool command_pool;

  VkCommandPoolCreateInfo pool_create_info = {};
  pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_create_info.queueFamilyIndex =
    plan->device->_protected->queue_families_for_queues[plan->device->_protected->graphics_queue_indices[0]];
  pool_create_info.flags = 0;

  if (vkCreateCommandPool(vk_device, &pool_create_info, NULL, &command_pool) != VK_SUCCESS) {
    printf("Failed to create command pool\n");
    exit(0);
  }

  // TODO static hax
  // create command buffer for each swap chain image
  plan->_protected->command_buffers = malloc(sizeof(VkCommandBuffer)*swap_chain_image_count);
  VkCommandBuffer *command_buffers = plan->_protected->command_buffers;

  VkCommandBufferAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = command_pool;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = (uint32_t) swap_chain_image_count;

  if (vkAllocateCommandBuffers(vk_device, &alloc_info, command_buffers) != VK_SUCCESS) {
    printf("Failed to create command buffers\n");
  }

  // TODO static hax
  // TODO we're creating a different command buffer for each swap chain image?
  // pretty sure we can submit the same buffer for each one.....
  // set each command buffer to complete a basic render pass
  for (int i = 0; i < swap_chain_image_count; i++)  {
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = 0;
    begin_info.pInheritanceInfo = NULL;

    if (vkBeginCommandBuffer(command_buffers[i], &begin_info) != VK_SUCCESS) {
      printf("Failed to begin command buffer %i\n",i);
    }

    VkRenderPassBeginInfo render_pass_begin_info = {};
    render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin_info.renderPass = render_pass;
    render_pass_begin_info.framebuffer = swap_chain_frame_buffers[i];
    render_pass_begin_info.renderArea.offset = (VkOffset2D){0, 0};
    render_pass_begin_info.renderArea.extent = swap_chain_extent;

    VkClearValue clearColor = {0.0f,0.0f,0.0f,1.0f};
    render_pass_begin_info.clearValueCount = 1;
    render_pass_begin_info.pClearValues = &clearColor;

    vkCmdBeginRenderPass(command_buffers[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);
    vkCmdDraw(command_buffers[i],3,1,0,0);
    vkCmdEndRenderPass(command_buffers[i]);
    if (vkEndCommandBuffer(command_buffers[i]) != VK_SUCCESS) {
      printf("Failed to end command buffer\n");
      exit(0);
    }
  }

  return true;
}

static bool gfks_render_plan_execute(gfks_render_plan *plan){
  VkDevice vk_device = plan->device->_protected->vk_logical_device;

  // TODO static hax
  // create semaphores
  VkSemaphore image_available_semaphore;
  VkSemaphore render_finished_semaphore;

  VkSemaphoreCreateInfo semaphore_create_info = {};
  semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  if (vkCreateSemaphore(vk_device, &semaphore_create_info, NULL, &image_available_semaphore) != VK_SUCCESS) {
    printf("Failed to create semaphore\n");
    exit(0);
  }

  if (vkCreateSemaphore(vk_device, &semaphore_create_info, NULL, &render_finished_semaphore) != VK_SUCCESS) {
    printf("Failed to create semaphore\n");
    exit(0);
  }

  // ------------------
  // --- draw frame ---
  // ------------------

  VkSwapchainKHR swap_chain =
    plan->_protected->render_passes[0]->_protected->presentation_surface_data[0]->swap_chain;
  VkQueue graphics_queue =
    plan->device->_protected->queues[plan->device->_protected->graphics_queue_indices[0]];
  VkQueue present_queue =
    plan->device->_protected->queues[plan->device->_protected->presentation_queue_indices[0]];

  // TODO static hax
  // get next image
  uint32_t image_index;
  int vr = vkAcquireNextImageKHR(vk_device, swap_chain, UINT64_MAX, image_available_semaphore, VK_NULL_HANDLE, &image_index);

  printf("got result %i from vkAquireNextImageKHR (0 is success)\n",vr);
  printf("got image %i from vkAquireNextImageKHR\n",image_index);

  // TODO static hax
  // submit the command buffer
  VkSemaphore wait_semaphores[] = {image_available_semaphore};
  VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSemaphore signal_semaphores[] = {render_finished_semaphore};


  VkSubmitInfo submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = wait_semaphores;
  submit_info.pWaitDstStageMask = wait_stages;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &(plan->_protected->command_buffers[image_index]);
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = signal_semaphores;

  if (vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS) {
    printf("Failed to submit command buffer to queue\n");
    exit(0);
  }

  // TODO static hax
  // presentation
  VkSwapchainKHR swap_chains[] = {swap_chain};

  VkPresentInfoKHR present_info = {};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = signal_semaphores;
  present_info.swapchainCount = 1;
  present_info.pSwapchains = swap_chains;
  present_info.pImageIndices = &image_index;
  present_info.pResults = NULL;

  if (vkQueuePresentKHR(present_queue, &present_info)!= VK_SUCCESS) {
    printf("Failed to present image\n");
  }

  return true;
}

static void gfks_free_render_plan(gfks_render_plan *plan) {
  free(plan->_protected);
  free(plan);
}

static gfks_render_plan* init_struct() {
  gfks_render_plan *p = malloc(sizeof(gfks_render_plan));
  p->_protected = malloc(sizeof(gfks_render_plan_protected));
  p->device = NULL;
  p->context = NULL;
  p->_protected->render_passes = NULL;
  p->_protected->command_buffers = NULL;
  p->_protected->render_pass_count = 0;

  p->free = &gfks_free_render_plan;
  p->add_render_pass = &gfks_render_plan_add_render_pass;
  p->finalize = &gfks_render_plan_finalize_plan;
  p->execute = &gfks_render_plan_execute;
}

gfks_render_plan* gfks_create_render_plan(gfks_context *context, gfks_device *device) {
  gfks_render_plan *new_plan = init_struct();
  new_plan->device = device;
  new_plan->context = context;

  // Allocate render passes array
  // TODO allocate dynamically
  new_plan->_protected->render_passes = malloc(sizeof(gfks_render_pass *)*256);

  return new_plan;
}
