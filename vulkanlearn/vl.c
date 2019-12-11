#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <X11/X.h>    
#include <X11/Xlib.h> 
#include <vulkan/vulkan_xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <unistd.h>
#include <SDL2/SDL_syswm.h>
#include <graffiks/graffiks.h>
#include <graffiks/internal.h>

VkInstance vk_instance;
VkPhysicalDevice physical_device;
VkDevice device;
VkQueue graphics_queue;
VkQueue present_queue;
VkSurfaceKHR vk_surface;
VkImage *swap_chain_images;
VkSwapchainKHR swap_chain;
VkFormat swap_chain_image_format;
VkExtent2D swap_chain_extent;

typedef struct {
  void *data;
  int  *file_size;
} SlurpedFile;

SlurpedFile* slurpFile(char *filename) {
    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
      printf("Error opening file\n");
    }

    // get file size
    fseek(f, 0L, SEEK_END);
    int file_size = ftell(f);
    fseek(f, 0L, SEEK_SET);

    void *output = malloc(file_size);
    fread(output, 1, file_size, f);

    int *out_file_size = malloc(sizeof(int));
    *out_file_size = file_size;

    SlurpedFile *out = malloc(sizeof(SlurpedFile));
    out->data = output;
    out->file_size = out_file_size;

    return out;
}

void spitSlurpedFile(SlurpedFile *sf) {
  free(sf->data);
  free(sf->file_size);
  free(sf);
}

void *handle_gfks_error() {
    printf("Graffiks error: %i. Exiting.\n", gfks_latest_error);
    exit(0);
    return NULL;
}

void *handle_SDL_error() {
    printf("SDL error: %s\n",SDL_GetError());
    exit(0);
    return NULL;
}

void god_function() {
  // Create SDL window
  (SDL_Init(SDL_INIT_VIDEO) == 0) || handle_SDL_error();

  SDL_Window *window;
  (window = SDL_CreateWindow("VulkanTriangleTest", 0, 0, 1024, 768,
                             SDL_WINDOW_VULKAN)) || handle_SDL_error();

  // Get SDL wm info
  SDL_SysWMinfo wmInfo;
  SDL_VERSION(&wmInfo.version);
  (SDL_GetWindowWMInfo(window, &wmInfo) == SDL_TRUE) || handle_SDL_error();

  // Create Graffiks context
  gfks_window_system window_systems = GFKS_WINDOW_SYSTEM_X11_BITFLAG;

  gfks_context *gfks_context;
  (gfks_context = gfks_create_context(&window_systems)) || handle_gfks_error();

  (window_systems & GFKS_WINDOW_SYSTEM_X11_BITFLAG) || handle_gfks_error();

  // Create surface
  gfks_surface *gfks_surface;
  (gfks_surface =
   gfks_create_surface_X11(gfks_context,
                           wmInfo.info.x11.display,
                           wmInfo.info.x11.window)) || handle_gfks_error();

  // Get devices
  uint32_t gfks_device_count;
  gfks_device *gfks_devices;
  (gfks_devices =
   gfks_get_devices_suitable_for_surfaces(gfks_context,
                                          gfks_surface,
                                          1,
                                          &gfks_device_count)) || handle_gfks_error();

  // Create our shaders
  SlurpedFile* vertshader = slurpFile("vert.spv");
  SlurpedFile* fragshader = slurpFile("frag.spv");

  gfks_shader* gfks_shader_stages[2];

  gfks_shader_stages[0] = gfks_create_shader(vertshader->data, *(vertshader->file_size), "main", &(gfks_devices[0]), GFKS_SHADER_STAGE_VERTEX);
  gfks_shader_stages[1] = gfks_create_shader(fragshader->data, *(fragshader->file_size), "main", &(gfks_devices[0]), GFKS_SHADER_STAGE_FRAGMENT);

  // Create a render pass
  gfks_render_pass *gfks_render_pass = gfks_create_render_pass(gfks_context, &(gfks_devices[0]), 1024.0f, 768.0f);

  // Add our surface as a presentation surface for the render pass
  if (gfks_render_pass->add_presentation_surface(gfks_render_pass, gfks_surface) == -1) handle_gfks_error();

  // hack vulkan stuffs out of graffiks
  vk_instance = *(gfks_context->_protected->vk_instance);
  vk_surface = gfks_surface->_protected->vk_surface;
  device = gfks_devices[0]._protected->vk_logical_device;
  physical_device = gfks_devices[0]._protected->vk_physical_device;
  graphics_queue = gfks_devices[0]._protected->queues[gfks_devices[0]._protected->graphics_queue_indices[0]];
  present_queue = gfks_devices[0]._protected->queues[gfks_devices[0]._protected->presentation_queue_indices[0]];
  int graphics_queue_family = gfks_devices[0]._protected->queue_families_for_queues[gfks_devices[0]._protected->graphics_queue_indices[0]];
  swap_chain = gfks_render_pass->_protected->presentation_surface_data[0]->swap_chain;
  swap_chain_image_format = gfks_render_pass->_protected->presentation_surface_data[0]->surface_format.format;
  swap_chain_extent = gfks_render_pass->_protected->presentation_surface_data[0]->swap_chain_extent;
  uint32_t swap_chain_image_count = gfks_render_pass->_protected->presentation_surface_data[0]->swap_chain_image_count;
  VkImage *swap_chain_images = gfks_render_pass->_protected->presentation_surface_data[0]->swap_chain_images;
  VkImageView *swap_chain_image_views = gfks_render_pass->_protected->presentation_surface_data[0]->swap_chain_image_views;
  VkShaderModule vert_shader_module = gfks_shader_stages[0]->_protected->vk_shader_module;
  VkShaderModule frag_shader_module = gfks_shader_stages[1]->_protected->vk_shader_module;
  VkPipelineShaderStageCreateInfo vertex_shader_stage_create_info = gfks_shader_stages[0]->_protected->vk_shader_stage_create_info;
  VkPipelineShaderStageCreateInfo fragment_shader_stage_create_info = gfks_shader_stages[1]->_protected->vk_shader_stage_create_info;
  VkPipelineShaderStageCreateInfo shader_stages[] = {vertex_shader_stage_create_info, fragment_shader_stage_create_info};
  VkPipelineViewportStateCreateInfo view_port_state_create_info = gfks_render_pass->_protected->viewport_state_create_info;;

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

  // define rasterizer create info
  //
  // this should be defined as a component of the "material" when setting up render pass
  VkPipelineRasterizationStateCreateInfo rasterizer = {};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0F;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.depthBiasConstantFactor = 0.0f;
  rasterizer.depthBiasClamp = 0.0f;
  rasterizer.depthBiasSlopeFactor = 0.0f;

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

  // set up color blending info
  VkPipelineColorBlendAttachmentState color_blend_attachment = {};
  color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo color_blending = {};
  color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending.logicOpEnable = VK_FALSE;
  color_blending.attachmentCount = 1;
  color_blending.pAttachments = &color_blend_attachment;

  // set up pipeline layout
  VkPipelineLayout pipeline_layout;
  VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
  pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

  if (vkCreatePipelineLayout(device, &pipeline_layout_create_info, NULL, &pipeline_layout) != VK_SUCCESS) {
    printf("Failed to create pipeline layout\n");
  }

  // subpass dependancies
  VkSubpassDependency dep = {};
  dep.srcSubpass = VK_SUBPASS_EXTERNAL;
  dep.dstSubpass = 0;
  dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dep.srcAccessMask = 0;
  dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;


  // define render pass
  VkAttachmentDescription color_attachment = {};
  color_attachment.format = swap_chain_image_format;
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

  if (vkCreateRenderPass(device, &render_pass_create_info, NULL, &render_pass) != VK_SUCCESS) {
    printf("Error creating render pass\n");
    exit(0);
  }

  // create pipeline
  VkGraphicsPipelineCreateInfo pipeline_create_info = {}; 
  pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_create_info.stageCount = 2;
  pipeline_create_info.pStages = shader_stages;
  pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;
  pipeline_create_info.pInputAssemblyState = &input_assembly_state_create_info;
  pipeline_create_info.pViewportState = &view_port_state_create_info;
  pipeline_create_info.pRasterizationState = &rasterizer;
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

  if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_create_info, NULL, &graphics_pipeline) != VK_SUCCESS) {
    printf("Failed to create graphics pipeline\n");
    exit(0);
  }


  // create a framebuffer for each swapchain image view
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

    if (vkCreateFramebuffer(device, &frame_buffer_create_info, NULL, &swap_chain_frame_buffers[i]) != VK_SUCCESS) {
      printf("Failed to create framebuffer for swap buffer image %i\n", i);
      exit(0);
    }
  }

  // create command pool
  VkCommandPool command_pool;

  VkCommandPoolCreateInfo pool_create_info = {};
  pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_create_info.queueFamilyIndex = graphics_queue_family;
  pool_create_info.flags = 0;

  if (vkCreateCommandPool(device, &pool_create_info, NULL, &command_pool) != VK_SUCCESS) {
    printf("Failed to create command pool\n");
    exit(0);
  }

  // create command buffer for each swap chain image
  VkCommandBuffer command_buffers[swap_chain_image_count];

  VkCommandBufferAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = command_pool;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = (uint32_t) swap_chain_image_count;

  if (vkAllocateCommandBuffers(device, &alloc_info, command_buffers) != VK_SUCCESS) {
    printf("Failed to create command buffers\n"); 
  }

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

  // create semaphores
  VkSemaphore image_available_semaphore;    
  VkSemaphore render_finished_semaphore;

  VkSemaphoreCreateInfo semaphore_create_info = {};
  semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  if (vkCreateSemaphore(device, &semaphore_create_info, NULL, &image_available_semaphore) != VK_SUCCESS) {
    printf("Failed to create semaphore\n");
    exit(0);
  }

  if (vkCreateSemaphore(device, &semaphore_create_info, NULL, &render_finished_semaphore) != VK_SUCCESS) {
    printf("Failed to create semaphore\n");
    exit(0);
  }

  // ------------------
  // --- draw frame ---
  // ------------------

  //for (int i = 0; i < 5; i++) {

  // get next image
  uint32_t image_index; 
  int vr = vkAcquireNextImageKHR(device, swap_chain, UINT64_MAX, image_available_semaphore, VK_NULL_HANDLE, &image_index);

  printf("got result %i from vkAquireNextImageKHR (0 is success)\n",vr);
  printf("got image %i from vkAquireNextImageKHR\n",image_index);

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
  submit_info.pCommandBuffers = &command_buffers[image_index];
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = signal_semaphores;

  if (vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS) {
    printf("Failed to submit command buffer to queue\n");
    exit(0);
  } 

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
  //}
  //

  // free_devices commented out to prevent validation layer from freaking out
  //  gfks_free_devices(gfks_devices, gfks_device_count); 
  gfks_surface->free(gfks_surface);
  gfks_context->free(gfks_context);
}


void done(){
  free(swap_chain_images);
}

void main() {
  god_function();
  sleep(10);
  done();
}
