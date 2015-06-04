#include "graffiks/governor.h"
#include "jni.h"
#include <android/log.h>

void Java_com_wentam_graffiks_GraffiksRenderer_on_1surface_1created(
    JNIEnv *e, jclass cls, jobject assetManager) {
  /*__android_log_print(ANDROID_LOG_INFO, "Graffiks","on_surface_created");*/
  _gfks_set_material_sys(e, assetManager);
  _gfks_init_graffiks();
}
void Java_com_wentam_graffiks_GraffiksRenderer_on_1surface_1changed(JNIEnv *env,
                                                                    jclass cls,
                                                                    jint width,
                                                                    jint height) {
  _gfks_set_size((int)width, (int)height);
}

void Java_com_wentam_graffiks_GraffiksRenderer_on_1draw_1frame(JNIEnv *env, jclass cls) {
  _gfks_draw_frame();
}

void Java_com_wentam_graffiks_GraffiksRenderer_on_1finish(JNIEnv *env, jclass cls) {
  _gfks_finish();
}
