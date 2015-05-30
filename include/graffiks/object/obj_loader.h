#include "graffiks/mesh/mesh.h"
#include <stdio.h>
#include <string.h>
#include "graffiks/object/object.h"
#include "graffiks/material/material.h"
#include "graffiks/renderer/renderer.h"

object *load_obj(renderer_flags flags, char *filepath);
