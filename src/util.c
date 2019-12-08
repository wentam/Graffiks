#include "graffiks/internal.h"

void gfks_err(gfks_error err, int debug_level, char *msg) {
  gfks_latest_error = err;

  if (debug_level <= GFKS_DEBUG_LEVEL) {
    printf("%s: ERROR: %s\n",GFKS_DEBUG_TAG,msg);
  }
}
