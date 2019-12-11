#include "graffiks/internal.h"

// Frees a gfks_shader
static void gfks_free_shader(gfks_shader *shader) {
  vkDestroyShaderModule(shader->device->_protected->vk_logical_device, shader->_protected->vk_shader_module, NULL);
  free(shader->_protected);
  free(shader);
}

// Allocates memory for our struct,
// assigns NULL to all pointer members,
// initializes primitive members,
// assigns method pointers,
// and returns the struct.
static gfks_shader* init_struct() {
  // Allocate memory
  gfks_shader *new_shader = malloc(sizeof(gfks_shader));
  new_shader->_protected  = malloc(sizeof(gfks_shader_protected));

  // Initialize all pointers to NULL
  new_shader->device = NULL;

  // Assign method pointers
  new_shader->free = &gfks_free_shader;

  return new_shader;
}


gfks_shader* gfks_create_shader(void *SPIRV_data,
                                uint32_t SPIRV_data_size,
                                char *entry_func_name,
                                gfks_device *device,
                                gfks_shader_stage shader_stage) {
  // Validate input
  if (SPIRV_data == NULL ||
      device     == NULL   ) gfks_err(GFKS_ERROR_NULL_ARGUMENT, 1,
                                      "gfks_create_shader received a NULL "
                                      "pointer as a required parameter");

  // Set up struct
  gfks_shader* new_shader  = init_struct();
  new_shader->shader_stage = shader_stage;
  new_shader->device       = device;

  // Create vulkan shader module
  VkShaderModule shader_module;
  VkShaderModuleCreateInfo shader_module_create_info = {};

  shader_module_create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shader_module_create_info.codeSize = SPIRV_data_size;
  shader_module_create_info.pCode    = (uint32_t *) SPIRV_data;

  VkResult r = vkCreateShaderModule(device->_protected->vk_logical_device,
                                    &shader_module_create_info,
                                    NULL, &shader_module);

  if (r != VK_SUCCESS) {
    if (r == VK_ERROR_INVALID_SHADER_NV) {
      gfks_err(GFKS_ERROR_INVALID_SPIRV_DATA, 1, "Attempted to create shader with invalid SPIR-V data");
    } else {
      gfks_err(GFKS_ERROR_UNKNOWN_VULKAN, 1, "Vulkan error");
    }
    new_shader->free(new_shader);
    return NULL;
  }

  // Assign our shader module
  new_shader->_protected->vk_shader_module = shader_module;

  // Create vulkan shader stage info
  VkPipelineShaderStageCreateInfo shader_stage_create_info = {};
  shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shader_stage_create_info.module = shader_module;
  shader_stage_create_info.pName = entry_func_name;

  switch (shader_stage) {
    case GFKS_SHADER_STAGE_VERTEX:
      shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
      break;
    case GFKS_SHADER_STAGE_FRAGMENT:
      shader_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
      break;
    case GFKS_SHADER_STAGE_GEOMETRY:
      shader_stage_create_info.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
      break;
    case GFKS_SHADER_STAGE_COMPUTE:
      shader_stage_create_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
      break;
  }

  // Assign our shader stage info to our struct
  new_shader->_protected->vk_shader_stage_create_info = shader_stage_create_info;

  // Return our new shader
  return new_shader;
}
