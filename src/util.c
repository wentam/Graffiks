#include "graffiks/internal.h"

// TODO make this stuff static and include .c directly?
// atm this stuff ends up in engine user's .o

void gfks_err(gfks_error err, int debug_level, char *msg) {
  gfks_latest_error = err;

  if (debug_level <= GFKS_DEBUG_LEVEL) {
    printf("%s: ERROR: %s\n",GFKS_DEBUG_TAG,msg);
  }
}

// Pass a recently allocated pointer and we will error if it's NULL and return
// true.
//
// False if no error
//
// This allows you to check for memory allocation errors with one line.
//
// Example:
// if (gfks_check_alloc_throw_err(ptr)) return false;
bool gfks_check_alloc_throw_err(void *ptr, char *file, int line) {
  if (ptr == NULL) {
    char error[1000];
    sprintf(error,"Memory allocation error in file %s on line %i\n",file,line);

    gfks_err(GFKS_ERROR_FAILED_MEMORY_ALLOCATION,
             1,
             error);

    return true; 
  }
  return false;
}
