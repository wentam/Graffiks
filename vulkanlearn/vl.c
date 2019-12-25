#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <SDL2/SDL_syswm.h>
#include <unistd.h>
#include <graffiks/graffiks.h>

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

void main() {
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


  /////////////////////////////////////
  // Create our subpass
  gfks_subpass* triangle_subpass = gfks_create_subpass(gfks_context, &(gfks_devices[0]), 1024.0f, 768.0f);

  // Add our shader set to our subpass
  uint32_t shaderset_index = triangle_subpass->add_shader_set(triangle_subpass, 2, gfks_shader_stages);

  // Define our subpass input array
  gfks_subpass* subpasses[1];
  subpasses[0] = triangle_subpass;

  // Define our subpass dependencies
  uint32_t subpass_deps[] = {};

  // Create our render pass
  gfks_render_pass* triangle_render_pass = gfks_create_render_pass(gfks_context, gfks_devices+0, gfks_surface, subpasses, 1, subpass_deps, 0);
  ////////////////////////////////////


  // Draw our triangle!
  triangle_render_pass->execute(triangle_render_pass) || handle_gfks_error();

  // Give the user a few seconds to look at our pretty triangle...
  sleep(10);

  // Free stuff
  // free_devices commented out to prevent validation layer from freaking out
  //  gfks_free_devices(gfks_devices, gfks_device_count);
  gfks_surface->free(gfks_surface);
  gfks_context->free(gfks_context);
}



