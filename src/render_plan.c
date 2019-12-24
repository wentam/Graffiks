#include "graffiks/internal.h"

static void add_pass_to_pass_order(gfks_render_plan *plan,
                                   uint32_t pass_index,
                                   bool *pass_added,
                                   uint32_t *passes_added) {

  // If this pass has already been added, do nothing.
  if (pass_added[pass_index]) return;

  // Grab handle to our planned render pass
  planned_render_pass *pass = &(plan->_protected->planned_render_passes[pass_index]);

  // Recursively add any un-added dependencies
  if (pass->dep_count > 0) {
    for (int i = 0; i < pass->dep_count; i++) {
      if (!(pass_added[pass->deps[i]])) {
        add_pass_to_pass_order(plan, pass->deps[i], pass_added, passes_added);
      }
    }
  }

  // Add our pass to the list
  plan->_protected->render_pass_order[*passes_added] = pass_index;
  pass_added[pass_index] = true;
  (*passes_added)++;
}

static void determine_render_pass_order(gfks_render_plan *plan) {
  // If we had a past allocation here, free it
  if (plan->_protected->render_pass_order != NULL) {
    free(plan->_protected->render_pass_order);
  }

  // Allocate our array
  plan->_protected->render_pass_order = malloc(sizeof(uint32_t)*plan->_protected->render_pass_count);

  // We need to keep track of how many we add
  uint32_t passes_added = 0;

  // Create array of bools to represent if we've found a place for each pass
  bool pass_added[plan->_protected->render_pass_count];
  for (int i = 0; i < plan->_protected->render_pass_count; i++) {
    pass_added[i] = false;
  }

  // Add all of our passes to the list with our recursive function
  for (int i = 0; i < plan->_protected->render_pass_count; i++) {
    add_pass_to_pass_order(plan, i, pass_added, &passes_added);
  }
}

static uint32_t gfks_render_plan_add_render_pass(gfks_render_plan *plan, gfks_render_pass *pass) {
  uint32_t *pcount = &(plan->_protected->render_pass_count);
  plan->_protected->planned_render_passes[*pcount].render_pass = pass;
  (*pcount)++;

  return (*pcount)-1;
}

static void gfks_render_plan_add_render_pass_dependency(gfks_render_plan *plan,
                                                        uint32_t pass_index,
                                                        uint32_t pass_dep_index) {
  // TODO make sure indices are valid

  uint32_t *dep_count = &(plan->_protected->planned_render_passes[pass_index].dep_count);
  plan->_protected->planned_render_passes[pass_index].deps[*dep_count] = pass_dep_index;
  (*dep_count)++;

  determine_render_pass_order(plan); // TODO this might not be the best point to determine order.
}

static bool gfks_render_plan_finalize_plan(gfks_render_plan *plan) {
  VkDevice vk_device = plan->device->_protected->vk_logical_device;

  determine_render_pass_order(plan); // TODO this might not be the best point to determine order.

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

  if (vkCreatePipelineLayout(vk_device,
                             &pipeline_layout_create_info,
                             NULL,
                             &pipeline_layout) != VK_SUCCESS) {
    printf("Failed to create pipeline layout\n");
  }

  // TODO static hax
  // define color attachment
  VkAttachmentDescription color_attachment = {};
  color_attachment.format =
    plan->_protected->planned_render_passes[0].render_pass->_protected->presentation_surface_data[0]->surface_format.format;
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

  // TODO: There are some situations in which a graffiks render pass should be a full render pass,
  // and not just a subpass

  // Define each render pass as a subpass
  uint32_t subpass_count = plan->_protected->render_pass_count;
  VkSubpassDescription subpasses[plan->_protected->render_pass_count];
  for(int i = 0; i < plan->_protected->render_pass_count; i++) {
    uint32_t pass_index = plan->_protected->render_pass_order[i];
    planned_render_pass* planned_pass = &(plan->_protected->planned_render_passes[pass_index]);

    subpasses[i].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpasses[i].colorAttachmentCount = 1;
    subpasses[i].pColorAttachments = &color_attachment_ref;
    subpasses[i].inputAttachmentCount = 0;
    subpasses[i].pInputAttachments = NULL;
    subpasses[i].preserveAttachmentCount = 0;
    subpasses[i].pPreserveAttachments = NULL;
    subpasses[i].pResolveAttachments = NULL;
    subpasses[i].pDepthStencilAttachment = NULL;
  }

  // Figure out how many subpass dependencies we need to create
  uint32_t subpass_dep_count = 0;
  for(int i = 0; i < plan->_protected->render_pass_count; i++) {
    uint32_t pass_index = plan->_protected->render_pass_order[i];
    planned_render_pass* planned_pass = &(plan->_protected->planned_render_passes[pass_index]);

    subpass_dep_count += planned_pass->dep_count;
  }
 
  // Define subpass dependencies 
  VkSubpassDependency subpass_deps[subpass_dep_count];
  uint32_t created_subpass_dep_count = 0;
  for(int i = 0; i < plan->_protected->render_pass_count; i++) {
    uint32_t pass_index = plan->_protected->render_pass_order[i];
    planned_render_pass* planned_pass = &(plan->_protected->planned_render_passes[pass_index]);
  
    for (int j = 0; j < planned_pass->dep_count; j++) {
      subpass_deps[created_subpass_dep_count].srcSubpass = i;
      subpass_deps[created_subpass_dep_count].dstSubpass = planned_pass->deps[j];

      // TODO: currently we assume the dependency is on the color attachment.
      // dependencies should be specific to the attachment that they actually depend on.
      subpass_deps[created_subpass_dep_count].srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      subpass_deps[created_subpass_dep_count].srcAccessMask = 0;
      subpass_deps[created_subpass_dep_count].dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      subpass_deps[created_subpass_dep_count].dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;  

      created_subpass_dep_count++;
    }
  }

  // Create our render pass
  VkRenderPass render_pass;

  VkRenderPassCreateInfo render_pass_create_info = {};
  render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_create_info.attachmentCount = 1;
  render_pass_create_info.pAttachments = &color_attachment;
  render_pass_create_info.subpassCount = subpass_count;
  render_pass_create_info.pSubpasses = subpasses;
  render_pass_create_info.dependencyCount = created_subpass_dep_count;
  render_pass_create_info.pDependencies = subpass_deps;

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

  // ----------------------------------------------------
  // Create pipeline for each render pass "draw step"
  // Stored as a flattened 2d array:
  //  * Each row is a render pass, with rows ordered as they are in plan->_protected->render_pass_order
  //  * Each item in each row is a pipeline for a draw step,
  //    with order corrisponding to pass->draw_steps
  //
  // This storage means as we iterate over planned render passes in draw order,
  // and the draw steps inside, the index will correspond to an item in this array that matches the
  // draw step
  // ----------------------------------------------------

  // Count up total draw steps
  uint32_t total_draw_step_count = 0;
  for (int i = 0; i < plan->_protected->render_pass_count; i++) {
    uint32_t pass_index = plan->_protected->render_pass_order[i];
    planned_render_pass* planned_pass = &(plan->_protected->planned_render_passes[pass_index]);

    total_draw_step_count += planned_pass->render_pass->_protected->draw_step_count;
  }

  // Create our pipelines
  VkPipeline draw_step_pipelines[total_draw_step_count];
  uint32_t defined_draw_step_pipelines = 0;
  for (int i = 0; i < plan->_protected->render_pass_count; i++) {
    uint32_t pass_index = plan->_protected->render_pass_order[i];
    planned_render_pass* planned_pass = &(plan->_protected->planned_render_passes[pass_index]);

    for (int j = 0; j < planned_pass->render_pass->_protected->draw_step_count; j++) {
      draw_step *draw_step = planned_pass->render_pass->_protected->draw_steps[j];

      uint32_t shader_count = draw_step->shader_count;
      gfks_shader **shader_set = draw_step->shader_set;

      VkPipelineShaderStageCreateInfo vk_shader_set[shader_count];
      for (int k = 0; k < shader_count; k++) {
        vk_shader_set[k] = shader_set[k]->_protected->vk_shader_stage_create_info;
      }

      VkGraphicsPipelineCreateInfo pipeline_create_info = {};
      pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
      pipeline_create_info.stageCount = shader_count;
      pipeline_create_info.pStages = vk_shader_set;
      pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;
      pipeline_create_info.pInputAssemblyState = &input_assembly_state_create_info;
      pipeline_create_info.pViewportState =
        &(planned_pass->render_pass->_protected->viewport_state_create_info);

      pipeline_create_info.pRasterizationState = &(draw_step->rsettings->_protected->settings);
      pipeline_create_info.pMultisampleState = &(draw_step->msettings->_protected->settings);
      pipeline_create_info.pDepthStencilState = NULL;
      pipeline_create_info.pColorBlendState = &color_blending;
      pipeline_create_info.pDynamicState = NULL;
      pipeline_create_info.layout = pipeline_layout;
      pipeline_create_info.renderPass = render_pass;
      pipeline_create_info.subpass = i;
      pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
      pipeline_create_info.basePipelineIndex = -1;

      VkPipeline pipeline;

      if (vkCreateGraphicsPipelines(vk_device,
                                    VK_NULL_HANDLE,
                                    1,
                                    &pipeline_create_info,
                                    NULL,
                                    &pipeline) != VK_SUCCESS) {
        // TODO error
      }

      draw_step_pipelines[defined_draw_step_pipelines++] = pipeline;
    }
  }

  // --------------------------------
  // Create a framebuffer for each swapchain image view in each presentation surface
  // --------------------------------

  // Count up presentation surfaces for array allocation
  uint32_t total_presentation_surface_count = 0;
  uint32_t max_presentation_surface_count = 0;
  for (int i = 0; i < plan->_protected->render_pass_count; i++) {
    uint32_t pass_index = plan->_protected->render_pass_order[i];
    planned_render_pass* planned_pass = &(plan->_protected->planned_render_passes[pass_index]);

    uint32_t c = planned_pass->render_pass->_protected->presentation_surface_count;
    if (c > max_presentation_surface_count) {
      max_presentation_surface_count = c;
    }
    total_presentation_surface_count += c;
  }

  // Figure out our largest swap chain image count for array allocation
  uint32_t max_swapchain_image_count = 0;

  for (int i = 0; i < plan->_protected->render_pass_count; i++) {
    uint32_t pass_index = plan->_protected->render_pass_order[i];
    planned_render_pass* planned_pass = &(plan->_protected->planned_render_passes[pass_index]);

    for (int j = 0; j < planned_pass->render_pass->_protected->presentation_surface_count; j++) {
      uint32_t c =
        planned_pass->render_pass->_protected->presentation_surface_data[j]->swap_chain_image_count;

      if (c > max_swapchain_image_count) {
        max_swapchain_image_count = c;
      }
    }
  }

  // Declare our framebuffer array
  VkFramebuffer framebuffers[total_presentation_surface_count];

  // Declare a lookup table that will define which framebuffer goes to which presentation surface
  // Rows are render passes
  // Cols are presentation surfaces
  // Values are framebuffer indices
  uint32_t framebuffer_lookup_table
    [plan->_protected->render_pass_count]
    [max_presentation_surface_count]
    [max_swapchain_image_count];

  // Create our framebuffers, writing our framebuffer lookup table along the way
  uint32_t created_framebuffer_count = 0;
  for (int i = 0; i < plan->_protected->render_pass_count; i++) {
    uint32_t pass_index = plan->_protected->render_pass_order[i];
    planned_render_pass* planned_pass = &(plan->_protected->planned_render_passes[pass_index]);
    
    for (int j = 0; j < planned_pass->render_pass->_protected->presentation_surface_count; j++) {
      presentation_surface_data* presentation_surface_data =
        (planned_pass->render_pass->_protected->presentation_surface_data[j]);

      // Create a framebuffer for each swapchain image view
      uint32_t swap_chain_image_count = presentation_surface_data->swap_chain_image_count;
      VkImageView *swap_chain_image_views = presentation_surface_data->swap_chain_image_views;
      VkExtent2D swap_chain_extent = presentation_surface_data->swap_chain_extent;

      for (int k = 0; k < swap_chain_image_count; k++) {
        VkImageView attachments[] = {swap_chain_image_views[k]};

        VkFramebufferCreateInfo frame_buffer_create_info = {};
        frame_buffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frame_buffer_create_info.renderPass = render_pass;
        frame_buffer_create_info.attachmentCount =1;
        frame_buffer_create_info.pAttachments  = attachments;
        frame_buffer_create_info.width = swap_chain_extent.width;
        frame_buffer_create_info.height = swap_chain_extent.height;
        frame_buffer_create_info.layers = 1;

        if (vkCreateFramebuffer(vk_device,
                                &frame_buffer_create_info, 
                                NULL, 
                                &framebuffers[created_framebuffer_count]) != VK_SUCCESS) {
          // TODO error
        }

        framebuffer_lookup_table[i][j][k] = created_framebuffer_count;
        created_framebuffer_count++;
      }
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

  uint32_t swap_chain_image_count = 2; // TODO hax
  VkExtent2D swap_chain_extent = {};   // etc...
  swap_chain_extent.width = 1024;      // etc...
  swap_chain_extent.height = 728;      // etc

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
      // TODO error
    }

    VkRenderPassBeginInfo render_pass_begin_info = {};
    render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin_info.renderPass = render_pass;
    render_pass_begin_info.framebuffer = framebuffers[framebuffer_lookup_table[0][0][i]];
    render_pass_begin_info.renderArea.offset = (VkOffset2D){0, 0};
    render_pass_begin_info.renderArea.extent = swap_chain_extent;

    VkClearValue clearColor = {0.0f,0.0f,0.0f,1.0f};
    render_pass_begin_info.clearValueCount = 1;
    render_pass_begin_info.pClearValues = &clearColor;

    vkCmdBeginRenderPass(command_buffers[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, draw_step_pipelines[0]);
    vkCmdDraw(command_buffers[i],3,1,0,0);
    vkCmdEndRenderPass(command_buffers[i]);
    if (vkEndCommandBuffer(command_buffers[i]) != VK_SUCCESS) {
      // TODO error
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

  if (vkCreateSemaphore(vk_device,
                        &semaphore_create_info,
                        NULL,
                        &image_available_semaphore) != VK_SUCCESS) {
    printf("Failed to create semaphore\n");
    exit(0);
  }

  if (vkCreateSemaphore(vk_device,
                        &semaphore_create_info,
                        NULL,
                        &render_finished_semaphore) != VK_SUCCESS) {
    printf("Failed to create semaphore\n");
    exit(0);
  }

  // ------------------
  // --- draw frame ---
  // ------------------

  VkSwapchainKHR swap_chain =
    plan->
    _protected->
    planned_render_passes[0].render_pass->
    _protected->
    presentation_surface_data[0]->
    swap_chain;

  VkQueue graphics_queue =
    plan->device->_protected->queues[plan->device->_protected->graphics_queue_indices[0]];
  VkQueue present_queue =
    plan->device->_protected->queues[plan->device->_protected->presentation_queue_indices[0]];

  // TODO static hax
  // get next image
  uint32_t image_index;
  int vr = vkAcquireNextImageKHR(vk_device,
                                 swap_chain,
                                 UINT64_MAX,
                                 image_available_semaphore,
                                 VK_NULL_HANDLE,
                                 &image_index);

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
  if (plan->_protected->render_pass_order != NULL) {
    free(plan->_protected->render_pass_order);
  }
  free(plan->_protected);
  free(plan);
}

static gfks_render_plan* init_struct() {
  gfks_render_plan *p = malloc(sizeof(gfks_render_plan));
  p->_protected = malloc(sizeof(gfks_render_plan_protected));
  p->device = NULL;
  p->context = NULL;
  p->_protected->planned_render_passes = NULL;
  p->_protected->command_buffers = NULL;
  p->_protected->render_pass_count = 0;

  p->free = &gfks_free_render_plan;
  p->add_render_pass = &gfks_render_plan_add_render_pass;
  p->add_render_pass_dependency = &gfks_render_plan_add_render_pass_dependency;
  p->finalize = &gfks_render_plan_finalize_plan;
  p->execute = &gfks_render_plan_execute;
}

gfks_render_plan* gfks_create_render_plan(gfks_context *context, gfks_device *device) {
  gfks_render_plan *new_plan = init_struct();
  new_plan->device = device;
  new_plan->context = context;
  new_plan->_protected->render_pass_order = NULL;

  // Allocate/init render passes array
  // TODO allocate dynamically
  new_plan->_protected->planned_render_passes = malloc(sizeof(planned_render_pass)*256);

  for (int i = 0; i < 256; i++) {
    new_plan->_protected->planned_render_passes[i].dep_count = 0;
  }

  return new_plan;
}
