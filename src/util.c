#include "graffiks/internal.h"

gfks_err(gfks_error err, int debug_level, char *file, int line, char *msg) {
  gfks_latest_error = err;

  if (debug_level >= GFKS_DEBUG_LEVEL) {
    printf("%s: Error in %s on line %i: %s\n",GFKS_DEBUG_TAG,file,line,msg);
  }
}
