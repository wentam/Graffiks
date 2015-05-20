#include "governor.h"

// stuff for calculating update time
#define MOVING_AVERAGE_PERIOD 60
#define SMOOTH_FACTOR 0.1
long long int frame_start_time = 0;
long long int last_frame_end_time = 0;
float delta_time_smoothed = 16.6;
float delta_time_moving_average = 16.6;

float ambient_color[] = {0,0,0,0};

int renderer_width;
int renderer_height;

void _init_graffiks() {
    glClearColor(0.0,0.0,0.0,1.0);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    // set up view matrix
    set_view_matrix(view_matrix, 0, 0, 7,  // camera location
                                0, 0, 0,  // camera target
                                 0, 1, 0); // up vector

    _call_init(&renderer_width, &renderer_height);
    _ms(&frame_start_time);
}

void _set_size(int width, int height) {
    glViewport(0,0,width,height);

    float ratio = (float) width/height;
    set_projection_matrix(projection_matrix,
                          -ratio*3, ratio*3, // left, right
                          3, -3,             // top, bottom
                          4, 100);           // near, far

    renderer_width = width;
    renderer_height = height;
}

void _draw_frame() {
    _limit_fps(60);


    // update
    _call_update(delta_time_smoothed);

    // clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw
    _call_draw();

#ifndef ANDROID
glXSwapBuffers(display, win);
#endif

    // figure out how much time the next update should handle
    // we use a moving average with an extra "smooth" pass to make the framerate more consistant
    long long int frame_end_time_;
    _ms(&frame_end_time_);

    float frame_time_elapsed;
    if (last_frame_end_time > 0) {
        frame_time_elapsed = frame_end_time_ - last_frame_end_time;
    } else {
        frame_time_elapsed = delta_time_smoothed;
    }

    delta_time_moving_average = (frame_time_elapsed+delta_time_moving_average*(MOVING_AVERAGE_PERIOD-1))/MOVING_AVERAGE_PERIOD;
    delta_time_smoothed = delta_time_smoothed+(delta_time_moving_average-delta_time_smoothed)*SMOOTH_FACTOR;
    last_frame_end_time = frame_end_time_;
}

void _finish() {
    _call_finish();
}

void set_camera_location_target_and_up(float x, float y, float z,
                                       float tx, float ty, float tz,
                                       float ux, float uy, float uz) {

    set_view_matrix(view_matrix, x,  y,  z,   // camera location
                                 tx, ty, tz,  // camera target
                                 ux, uy, uz); // up vector
}

void _limit_fps(int fps) {
    int ms_per_frame = 1000/fps;

    long long int frame_end_time;
    _ms(&frame_end_time);

    int frame_delta_time = frame_end_time-frame_start_time;
    if (frame_delta_time < ms_per_frame) {
        _sleep_ms(ms_per_frame-frame_delta_time);
    }

    _ms(&frame_start_time);
}

void _sleep_ms(int ms) {
    struct timespec req = {0};
    req.tv_sec = 0;
    req.tv_nsec = ms * 1000000L;
    nanosleep(&req, (struct timespec *)NULL);
}

// current epoch in milliseconds
void _ms(long long int *ms) {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    (*ms) = llround((tp.tv_sec*1000.0)+(tp.tv_usec/1000.0));
}