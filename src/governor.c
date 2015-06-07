#include "graffiks/governor.h"
#include "graffiks/renderer/renderer.h"
#include "graffiks/renderer/deferred_renderer.h"
#include "graffiks/renderer/forward_renderer.h"
#include "graffiks/lights.h"
#include "graffiks/camera.h"
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

// stuff for calculating update time
#define MOVING_AVERAGE_PERIOD 60
#define SMOOTH_FACTOR 0.1
long long int frame_start_time = 0;
long long int last_frame_end_time = 0;
float delta_time_smoothed = 16.6;
float delta_time_moving_average = 16.6;

#ifdef _WIN32
void _sleep_ms(int ms) { Sleep(ms); }

void _ms(long long int *ms) { *ms = GetTickCount64(); }
#endif
#ifndef _WIN32
void _sleep_ms(int ms) {
  struct timespec req = {0};
  req.tv_sec = 0;
  req.tv_nsec = ms * 1000000L;
  nanosleep(&req, (struct timespec *)NULL);
}

void _ms(long long int *ms) {
  struct timeval tp;
  gettimeofday(&tp, NULL);
  (*ms) = llround((tp.tv_sec * 1000.0) + (tp.tv_usec / 1000.0));
}
#endif

void _limit_fps(int fps) {
  int ms_per_frame = 1000 / fps;

  long long int frame_end_time;
  _ms(&frame_end_time);

  int frame_delta_time = frame_end_time - frame_start_time;
  if (frame_delta_time < ms_per_frame) {
    _sleep_ms(ms_per_frame - frame_delta_time);
    //    frame_delta_time += ms_per_frame - frame_delta_time;
  }

  //  printf("framerate: %f\n", 1000.0f / frame_delta_time);

  _ms(&frame_start_time);
}

void _gfks_init() {
  _gfks_call_init(&renderer_width, &renderer_height);
  _ms(&frame_start_time);
}

void _gfks_draw_frame() {
  _limit_fps(250);

  // update
  _gfks_call_update(delta_time_smoothed);

  // clear screen
  _gfks_clear(gfks_enabled_renderers);

  // draw
  _gfks_call_draw();

  gfks_draw_objects();

  // figure out how much time the next update should handle
  // we use a moving average with an extra "smooth" pass to make the framerate more
  // consistant
  long long int frame_end_time_;
  _ms(&frame_end_time_);

  float frame_time_elapsed;
  if (last_frame_end_time > 0) {
    frame_time_elapsed = frame_end_time_ - last_frame_end_time;
  } else {
    frame_time_elapsed = delta_time_smoothed;
  }

  delta_time_moving_average =
      (frame_time_elapsed + delta_time_moving_average * (MOVING_AVERAGE_PERIOD - 1)) /
      MOVING_AVERAGE_PERIOD;
  delta_time_smoothed = delta_time_smoothed +
                        (delta_time_moving_average - delta_time_smoothed) * SMOOTH_FACTOR;
  last_frame_end_time = frame_end_time_;
}

void _gfks_finish() { _gfks_call_finish(); }
