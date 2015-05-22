#include "mesh/mesh.h"
#include <stdio.h>
#include <string.h>
#include "object/object.h"
#include "material/material.h"
#include "renderer/renderer.h"

object *load_obj(renderer_flags flags, char *filepath);
