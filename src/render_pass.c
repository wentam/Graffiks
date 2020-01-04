#include "graffiks/internal.h"
#include "surface_utils.c"

static void add_subpass_to_subpass_order(gfks_render_pass *pass,
                                   uint32_t pass_index,
                                   bool *pass_added,
                                   uint32_t *passes_added) {

  // If this pass has already been added, do nothing.
  if (pass_added[pass_index]) return;

  // Grab handle to our planned render pass
  planned_subpass *subpass = &(pass->_protected->planned_subpasses[pass_index]);

  // Recursively add any un-added dependencies
  if (subpass->dep_count > 0) {
    for (int i = 0; i < subpass->dep_count; i++) {
      if (!(pass_added[subpass->deps[i]])) {
        add_subpass_to_subpass_order(pass, subpass->deps[i], pass_added, passes_added);
      }
    }
  }

  // Add our pass to the list
  pass->_protected->subpass_order[*passes_added] = pass_index;
  pass_added[pass_index] = true;
  (*passes_added)++;
}

static void determine_subpass_order(gfks_render_pass *pass) {
  // If we had a past allocation here, free it
  if (pass->_protected->subpass_order != NULL) {
    free(pass->_protected->subpass_order);
  }

  // Allocate our array
  pass->_protected->subpass_order = malloc(sizeof(uint32_t)*pass->_protected->subpass_count);

  // We need to keep track of how many we add
  uint32_t passes_added = 0;

  // Create array of bools to represent if we've found a place for each pass
  bool pass_added[pass->_protected->subpass_count];
  for (int i = 0; i < pass->_protected->subpass_count; i++) {
    pass_added[i] = false;
  }

  // Add all of our passes to the list with our recursive function
  for (int i = 0; i < pass->_protected->subpass_count; i++) {
    add_subpass_to_subpass_order(pass, i, pass_added, &passes_added);
  }
}

static bool set_up_presentation_surface(gfks_render_pass *render_pass) {


  // Grab easy handle to presentation surface
  presentation_surface *ps = &(render_pass->_protected->presentation_surface);

  // Grab our surface and device
  gfks_surface *surface = ps->surface;
  gfks_device *device = render_pass->device;

  // Define our surface capabilities
  if (!gfks_surface_util_obtain_surface_capabilities_for_device(surface,
                                                                device,
                                                                &(ps->surface_capabilities))) {
    return false;
  }

  // Decide swap chain image count
  uint32_t desired_swap_chain_image_count =
    gfks_surface_util_decide_desired_swap_chain_image_count(ps->surface_capabilities);

  // Decide swap chain image size
  ps->swap_chain_extent =
    gfks_surface_util_decide_swap_chain_image_size(ps->surface_capabilities, surface);

  // Decide surface format
  if (!gfks_surface_util_decide_surface_format_for_device(surface,
                                                          device,
                                                          &(ps->surface_format))) return false;

  // Decide presentation mode
  if (!gfks_surface_util_decide_surface_presentation_mode_for_device(surface,
                                                                     device,
                                                                     &(ps->surface_presentation_mode))) return false;

  // Create our swap chain
  if(!gfks_surface_util_create_swap_chain(surface,
                                          device,
                                          ps->surface_capabilities,
                                          ps->surface_format,
                                          ps->surface_presentation_mode,
                                          ps->swap_chain_extent,
                                          &(ps->swap_chain),
                                          &(ps->swap_chain_image_count),
                                          &(ps->swap_chain_images),
                                          &(ps->swap_chain_image_views),
                                          desired_swap_chain_image_count)) return false;

  return true;
}


static uint32_t add_subpass(gfks_render_pass *pass, gfks_subpass *subpass) {
  uint32_t *pcount = &(pass->_protected->subpass_count);
  pass->_protected->planned_subpasses[*pcount].subpass = subpass;
  (*pcount)++;

  return (*pcount)-1;
}

static void add_subpass_dependency(gfks_render_pass *pass,
                                   uint32_t pass_index,
                                   uint32_t pass_dep_index) {
  // TODO make sure indices are valid
  uint32_t *dep_count = &(pass->_protected->planned_subpasses[pass_index].dep_count);
  pass->_protected->planned_subpasses[pass_index].deps[*dep_count] = pass_dep_index;
  (*dep_count)++;
}


static bool gfks_render_pass_execute(gfks_render_pass *pass){
  VkDevice vk_device = pass->device->_protected->vk_logical_device;

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
    pass->_protected->presentation_surface.swap_chain;

  VkQueue graphics_queue =
    pass->device->_protected->queues[pass->device->_protected->graphics_queue_indices[0]];
  VkQueue present_queue =
    pass->device->_protected->queues[pass->device->_protected->presentation_queue_indices[0]];

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
  submit_info.pCommandBuffers = &(pass->_protected->command_buffers[image_index]);
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

static void gfks_free_render_pass(gfks_render_pass *pass) {
  if (pass->_protected->subpass_order != NULL) {
    free(pass->_protected->subpass_order);
  }
  free(pass->_protected);
  free(pass);
}

static gfks_render_pass* init_struct() {
  gfks_render_pass *p = malloc(sizeof(gfks_render_pass));
  p->_protected = malloc(sizeof(gfks_render_pass_protected));
  p->device = NULL;
  p->context = NULL;
  p->_protected->planned_subpasses = NULL;
  p->_protected->command_buffers = NULL;
  p->_protected->subpass_count = 0;
  p->_protected->subpass_order = NULL;

  // Assign method pointers
  p->free = &gfks_free_render_pass;
  p->execute = &gfks_render_pass_execute;
}

gfks_render_pass* gfks_create_render_pass(gfks_context *context,
                                          gfks_device *device,
                                          gfks_surface *surface,
                                          gfks_subpass **subpasses,
                                          uint32_t subpass_count,
                                          uint32_t* subpass_deps,
                                          uint32_t subpass_dep_count) {

  // Init our struct
  gfks_render_pass *pass = init_struct();

  // Assign our device, context
  pass->device = device;
  pass->context = context;

  // Assign and set up our presentation surface
  pass->_protected->presentation_surface.surface = surface;
  if (!set_up_presentation_surface(pass)) return NULL;

  // Allocate planned subpasses
  pass->_protected->planned_subpasses = malloc(sizeof(planned_subpass)*subpass_count);

  if (pass->_protected->planned_subpasses == NULL) {
    // TODO error
  }

  // Define/init our planned subpasses
  for (int i = 0; i < subpass_count; i++) {
    pass->_protected->planned_subpasses[i].dep_count = 0;
    pass->_protected->planned_subpasses[i].subpass = subpasses[i];
  }

  pass->_protected->subpass_count = subpass_count;

  // Set up subpass dependencies
  // Dependencies are defined as a flattened 2d array with 2 columns, hence i+=2
  for (int i = 0; i < subpass_dep_count; i+=2) {
    add_subpass_dependency(pass, subpass_deps[i], subpass_deps[i+1]);     
  }

  // Figure out what order we will run our subpasses based on dependencies
  determine_subpass_order(pass);

  // Grab easy vulkan logical device handle
  VkDevice vk_device = device->_protected->vk_logical_device;

  // TODO static hax
  // define color attachment
  VkAttachmentDescription color_attachment = {};
  color_attachment.format =
    pass->_protected->presentation_surface.surface_format.format;
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

  // Define each subpass
  VkSubpassDescription vk_subpasses[pass->_protected->subpass_count];
  for(int i = 0; i < pass->_protected->subpass_count; i++) {
    uint32_t pass_index = pass->_protected->subpass_order[i];
    planned_subpass* planned_pass = &(pass->_protected->planned_subpasses[pass_index]);

    vk_subpasses[i].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    vk_subpasses[i].colorAttachmentCount = 1;
    vk_subpasses[i].pColorAttachments = &color_attachment_ref;
    vk_subpasses[i].inputAttachmentCount = 0;
    vk_subpasses[i].pInputAttachments = NULL;
    vk_subpasses[i].preserveAttachmentCount = 0;
    vk_subpasses[i].pPreserveAttachments = NULL;
    vk_subpasses[i].pResolveAttachments = NULL;
    vk_subpasses[i].pDepthStencilAttachment = NULL;
  }
 
  // Define subpass dependencies 
  VkSubpassDependency vk_subpass_deps[subpass_dep_count];
  uint32_t created_subpass_dep_count = 0;
  for(int i = 0; i < pass->_protected->subpass_count; i++) {
    uint32_t pass_index = pass->_protected->subpass_order[i];
    planned_subpass* planned_pass = &(pass->_protected->planned_subpasses[pass_index]);
  
    for (int j = 0; j < planned_pass->dep_count; j++) {
      vk_subpass_deps[created_subpass_dep_count].srcSubpass = i;
      vk_subpass_deps[created_subpass_dep_count].dstSubpass = planned_pass->deps[j];

      // TODO: currently we assume the dependency is on the color attachment.
      // dependencies should be specific to the attachment that they actually depend on.
      vk_subpass_deps[created_subpass_dep_count].srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      vk_subpass_deps[created_subpass_dep_count].srcAccessMask = 0;
      vk_subpass_deps[created_subpass_dep_count].dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      vk_subpass_deps[created_subpass_dep_count].dstAccessMask =
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
  render_pass_create_info.pSubpasses = vk_subpasses;
  render_pass_create_info.dependencyCount = created_subpass_dep_count;
  render_pass_create_info.pDependencies = vk_subpass_deps;

  if (vkCreateRenderPass(vk_device, &render_pass_create_info, NULL, &render_pass) != VK_SUCCESS) {
    // TODO error
  }

  // Create command pool
  VkCommandPool command_pool;

  VkCommandPoolCreateInfo pool_create_info = {};
  pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_create_info.queueFamilyIndex =
    pass->device->_protected->queue_families_for_queues[pass->device->_protected->graphics_queue_indices[0]];
  pool_create_info.flags = 0;

  if (vkCreateCommandPool(vk_device, &pool_create_info, NULL, &command_pool) != VK_SUCCESS) {
    // TODO error
  }

  // --------------------------------
  // Create a framebuffer for each swapchain image view in our presentation surface
  // --------------------------------

  // Declare our framebuffer array
  VkFramebuffer framebuffers[pass->_protected->presentation_surface.swap_chain_image_count];

  // Create our framebuffers
  uint32_t created_framebuffer_count = 0;

  // Create a framebuffer for each swapchain image view
  uint32_t swap_chain_image_count = pass->_protected->presentation_surface.swap_chain_image_count;
  VkImageView *swap_chain_image_views = pass->_protected->presentation_surface.swap_chain_image_views;
  VkExtent2D swap_chain_extent = pass->_protected->presentation_surface.swap_chain_extent;

  for (int i = 0; i < swap_chain_image_count; i++) {
    VkImageView attachments[] = {swap_chain_image_views[i]};

    VkFramebufferCreateInfo frame_buffer_create_info = {};
    frame_buffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frame_buffer_create_info.renderPass = render_pass;
    frame_buffer_create_info.attachmentCount = 1;
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

    created_framebuffer_count++;
  }

  // Set up our subpasses for this render pass based on subpass_order
  uint32_t render_pass_data_indices[pass->_protected->subpass_count];

  for(int i = 0; i < pass->_protected->subpass_count; i++) {
    uint32_t pass_index = pass->_protected->subpass_order[i];
    gfks_subpass* subpass = pass->_protected->planned_subpasses[i].subpass;
  
    render_pass_data_indices[i] = subpass->_protected->set_up_for_render_pass(subpass,
                                                                              pass,
                                                                              render_pass,
                                                                              command_pool,
                                                                              pass_index);
  } 
  
  // Create command buffer for each swap chain image
  pass->_protected->command_buffers = malloc(sizeof(VkCommandBuffer)*swap_chain_image_count);
  VkCommandBuffer *command_buffers = pass->_protected->command_buffers;

  VkCommandBufferAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = command_pool;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = (uint32_t) swap_chain_image_count;

  if (vkAllocateCommandBuffers(vk_device, &alloc_info, command_buffers) != VK_SUCCESS) {
    // TODO error
  }

  // Set each command buffer to complete a basic render pass
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
    render_pass_begin_info.framebuffer = framebuffers[i];
    render_pass_begin_info.renderArea.offset = (VkOffset2D){0, 0};
    render_pass_begin_info.renderArea.extent = swap_chain_extent;

    VkClearValue clearColor = {0.0f,0.0f,0.0f,1.0f};
    render_pass_begin_info.clearValueCount = 1;
    render_pass_begin_info.pClearValues = &clearColor;

    vkCmdBeginRenderPass(command_buffers[i],
                         &render_pass_begin_info,
                         VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

    // Loop over subpasses
    for (int j = 0; j < pass->_protected->subpass_count; j++) {
      // We need to use our subpasses in the order defined in subpass_order to satisfy dependencies
      uint32_t pass_index = pass->_protected->subpass_order[j];
      gfks_subpass* subpass = pass->_protected->planned_subpasses[j].subpass;

      // Execute secondary command buffer for this subpass
      // TODO render pass data index should not be static
      vkCmdExecuteCommands(command_buffers[i],
                           1,
                           &(subpass->_protected->render_pass_data[render_pass_data_indices[j]].vk_command_buffer));

      // If this is not the last subpass, call vkCmdNextSubpass
      if (j < (pass->_protected->subpass_count)-1) {
        vkCmdNextSubpass(command_buffers[i], VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
      }
    }

    vkCmdEndRenderPass(command_buffers[i]);
    if (vkEndCommandBuffer(command_buffers[i]) != VK_SUCCESS) {
      // TODO error
    }
  }

  return pass;
}
