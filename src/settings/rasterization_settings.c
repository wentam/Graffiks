#include "graffiks/internal.h"

static void gfks_rasterization_settings_front_face_clockwise(gfks_rasterization_settings *settings) {
  settings->_protected->settings.frontFace = VK_FRONT_FACE_CLOCKWISE;
}

static void gfks_rasterization_settings_front_face_counterclockwise(gfks_rasterization_settings *settings) {
  settings->_protected->settings.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
}

static void gfks_rasterization_settings_depth_clamp_enabled(gfks_rasterization_settings *settings,
                                                                bool enable) {
  settings->_protected->settings.depthClampEnable = enable ? VK_TRUE : VK_FALSE;
}

static void gfks_rasterization_settings_rasterizer_discard_enabled(gfks_rasterization_settings *settings,
                                                                       bool enable) {
  settings->_protected->settings.rasterizerDiscardEnable = enable ? VK_TRUE : VK_FALSE;
}

static void gfks_rasterization_settings_polygon_mode_fill(gfks_rasterization_settings *settings) {
  settings->_protected->settings.polygonMode = VK_POLYGON_MODE_FILL;
}

static void gfks_rasterization_settings_polygon_mode_line(gfks_rasterization_settings *settings) {
  settings->_protected->settings.polygonMode = VK_POLYGON_MODE_LINE;
}

static void gfks_rasterization_settings_polygon_mode_point(gfks_rasterization_settings *settings) {
  settings->_protected->settings.polygonMode = VK_POLYGON_MODE_POINT;
}

static void gfks_rasterization_settings_line_width(gfks_rasterization_settings *settings,
                                                       float line_width) {
  settings->_protected->settings.lineWidth = line_width;
}

static void gfks_rasterization_settings_disable_culling(gfks_rasterization_settings *settings) {
  settings->_protected->settings.cullMode = VK_CULL_MODE_NONE;
}

static void gfks_rasterization_settings_culling_front(gfks_rasterization_settings *settings) {
  settings->_protected->settings.cullMode = VK_CULL_MODE_FRONT_BIT;
}

static void gfks_rasterization_settings_culling_back(gfks_rasterization_settings *settings) {
  settings->_protected->settings.cullMode = VK_CULL_MODE_BACK_BIT;
}

static void gfks_rasterization_settings_culling_frontback(gfks_rasterization_settings *settings) {
  settings->_protected->settings.cullMode = VK_CULL_MODE_FRONT_AND_BACK;
}

static void gfks_rasterization_settings_depth_bias_enabled(gfks_rasterization_settings *settings,
                                                               bool setting) {
  settings->_protected->settings.depthBiasEnable = setting ? VK_TRUE : VK_FALSE;
}

static void gfks_rasterization_settings_depth_bias_constant_factor(gfks_rasterization_settings *settings,
                                                                       float setting) {
  settings->_protected->settings.depthBiasConstantFactor = setting;
}

static void gfks_rasterization_settings_depth_bias_clamp(gfks_rasterization_settings *settings,
                                                             float setting) {
  settings->_protected->settings.depthBiasClamp = setting;
}

static void gfks_rasterization_settings_depth_bias_slope_factor(gfks_rasterization_settings *settings,
                                                                    float setting) {
  settings->_protected->settings.depthBiasSlopeFactor = setting;
}

static void gfks_free_rasterization_settings(gfks_rasterization_settings *settings) {
  free(settings->_protected);
  free(settings);
}

static gfks_rasterization_settings* init_struct() {
  gfks_rasterization_settings *r = malloc(sizeof(gfks_rasterization_settings));
  r->_protected = malloc(sizeof(gfks_rasterization_settings_protected));

  r->free = &gfks_free_rasterization_settings;
  r->front_face_clockwise        = &gfks_rasterization_settings_front_face_clockwise;
  r->front_face_counterclockwise = &gfks_rasterization_settings_front_face_counterclockwise;
  r->depth_clamp_enabled         = &gfks_rasterization_settings_depth_clamp_enabled;
  r->rasterizer_discard_enabled  = &gfks_rasterization_settings_rasterizer_discard_enabled;
  r->polygon_mode_fill           = &gfks_rasterization_settings_polygon_mode_fill;
  r->polygon_mode_line           = &gfks_rasterization_settings_polygon_mode_line;
  r->polygon_mode_point          = &gfks_rasterization_settings_polygon_mode_point;
  r->line_width                  = &gfks_rasterization_settings_line_width;
  r->disable_culling             = &gfks_rasterization_settings_disable_culling;
  r->culling_front               = &gfks_rasterization_settings_culling_front;
  r->culling_back                = &gfks_rasterization_settings_culling_back;
  r->culling_frontback           = &gfks_rasterization_settings_culling_frontback;
  r->depth_bias_enabled          = &gfks_rasterization_settings_depth_bias_enabled;
  r->depth_bias_constant_factor  = &gfks_rasterization_settings_depth_bias_constant_factor;
  r->depth_bias_clamp            = &gfks_rasterization_settings_depth_bias_clamp;
  r->depth_bias_slope_factor     = &gfks_rasterization_settings_depth_bias_slope_factor;

  return r;
}

gfks_rasterization_settings* gfks_create_rasterization_settings() {
  gfks_rasterization_settings *new_obj = init_struct();

  new_obj->_protected->settings.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  new_obj->_protected->settings.depthClampEnable = VK_FALSE;
  new_obj->_protected->settings.rasterizerDiscardEnable = VK_FALSE;
  new_obj->_protected->settings.polygonMode = VK_POLYGON_MODE_FILL;
  new_obj->_protected->settings.lineWidth = 1.0F;
  new_obj->_protected->settings.cullMode = VK_CULL_MODE_BACK_BIT;
  new_obj->_protected->settings.frontFace = VK_FRONT_FACE_CLOCKWISE;
  new_obj->_protected->settings.depthBiasEnable = VK_FALSE;
  new_obj->_protected->settings.depthBiasConstantFactor = 0.0f;
  new_obj->_protected->settings.depthBiasClamp = 0.0f;
  new_obj->_protected->settings.depthBiasSlopeFactor = 0.0f;
  new_obj->_protected->settings.flags = 0;

  return new_obj;
}

