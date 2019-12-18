#include "graffiks/internal.h"

// TODO multisampling settings depend on a device. a gfks_device should be passed to create the
// settings. All setting changes should be verified as support by the device, including the default
// settings.
//
// gfks_device should then maintain a pointer to it's multisamping settings -- and automatically
// assigned at device creation. This way, the user just does stuff like:
// device->multisample_settings->sample_shading_enabled(device->multisample_settings, true)


static void gfks_multisample_settings_sample_shading_enabled(gfks_multisample_settings *settings,
                                                             bool setting) {
  
  settings->_protected->settings.sampleShadingEnable = setting ? VK_TRUE : VK_FALSE;
}

static void gfks_multisample_settings_rasterization_samples(gfks_multisample_settings *settings,
                                                            gfks_sample_count_bitflags flags) {
  
  settings->_protected->settings.rasterizationSamples = flags;
}

static void gfks_multisample_settings_minimum_sample_shading(gfks_multisample_settings *settings,
                                                             float setting) {
 
  if (setting > 0.0f && setting < 1.0f) {
    settings->_protected->settings.minSampleShading = setting;
  } else {
    // TODO error
  }
}

static void gfks_multisample_settings_sample_mask(gfks_multisample_settings *settings,
                                                  uint32_t *sample_mask) {
  
  settings->_protected->settings.pSampleMask = sample_mask;
}

static void gfks_multisample_settings_alpha_to_coverage_enabled(gfks_multisample_settings *settings,
                                                                bool setting) {
  
  settings->_protected->settings.alphaToCoverageEnable = setting ? VK_TRUE : VK_FALSE;
}

static void gfks_multisample_settings_alpha_to_one_enabled(gfks_multisample_settings *settings,
                                                                bool setting) {
  
  settings->_protected->settings.alphaToOneEnable = setting ? VK_TRUE : VK_FALSE;
}

static void gfks_free_multisample_settings(gfks_multisample_settings *settings) {
  free(settings->_protected);
  free(settings);
}

static gfks_multisample_settings* init_struct() {
  gfks_multisample_settings* ns = malloc(sizeof(gfks_multisample_settings));
  ns->_protected = malloc(sizeof(gfks_multisample_settings_protected));

  ns->free = &gfks_free_multisample_settings;
  ns->sample_shading_enabled = &gfks_multisample_settings_sample_shading_enabled;
  ns->rasterization_samples = &gfks_multisample_settings_rasterization_samples;
  ns->minimum_sample_shading = &gfks_multisample_settings_minimum_sample_shading;
  ns->sample_mask = &gfks_multisample_settings_sample_mask;
  ns->alpha_to_coverage_enabled = &gfks_multisample_settings_alpha_to_coverage_enabled;
  ns->alpha_to_one_enabled = &gfks_multisample_settings_alpha_to_one_enabled;

  return ns;
}

gfks_multisample_settings* gfks_create_multisample_settings() {
  gfks_multisample_settings* ns = init_struct();

  VkPipelineMultisampleStateCreateInfo defaults = {};
  defaults.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  defaults.sampleShadingEnable = VK_FALSE;
  defaults.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  defaults.minSampleShading = 1.0f;
  defaults.pSampleMask = NULL;
  defaults.alphaToCoverageEnable = VK_FALSE;
  defaults.alphaToOneEnable = VK_FALSE;

  ns->_protected->settings = defaults;

  return ns;
}
