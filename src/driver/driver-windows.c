#include <windows.h>
#include <GL/glew.h>
#include <GL/GL.h>
#include <stdlib.h>
#include "graffiks/driver/driver-windows.h"

int quit = 0;
int setup = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void init_graffiks_windows(int window_width, int window_height, char *window_title,
                           void (*init)(int *width, int *height),
                           void (*update)(float time_step), void (*finish)(void),
                           HINSTANCE hInstance) {
    wchar_t w_window_title[255];
    mbstowcs(w_window_title, window_title, 255);
    MSG msg          = {0};
	WNDCLASS wc      = {0}; 
	wc.lpfnWndProc   = WndProc;
	wc.hInstance     = hInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wc.lpszClassName = window_title;
	wc.style = CS_OWNDC;
	if( !RegisterClass(&wc) )
		;
	CreateWindowW(w_window_title,w_window_title,WS_OVERLAPPEDWINDOW|WS_VISIBLE,0,0,window_width,window_height,0,0,hInstance,0);
 
    while(!quit){
        if(PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ))
            DispatchMessage( &msg );
        if(1 == setup) {
            setup = 2;
            graffiks_setup(init, update, finish);
            _set_size(window_width, window_height);

            _init_graffiks();
        }
        if(2 == setup)
            _draw_frame();
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_CREATE:
		{
		PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
			PFD_TYPE_RGBA,            //The kind of framebuffer. RGBA or palette.
			32,                        //Colordepth of the framebuffer.
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			24,                        //Number of bits for the depthbuffer
			8,                        //Number of bits for the stencilbuffer
			0,                        //Number of Aux buffers in the framebuffer.
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};
 
		hdc = GetDC(hWnd);
 
		int  letWindowsChooseThisPixelFormat;
		letWindowsChooseThisPixelFormat = ChoosePixelFormat(hdc, &pfd); 
		SetPixelFormat(hdc,letWindowsChooseThisPixelFormat, &pfd);
 
		HGLRC ourOpenGLRenderingContext = wglCreateContext(hdc);
		wglMakeCurrent (hdc, ourOpenGLRenderingContext);
 
          GLenum err = glewInit();
          if (GLEW_OK != err) {
            printf("glew error: %s\n", glewGetErrorString(err));
          }

          setup = 1;
		}
		break;
    case WM_QUIT:
    case WM_CLOSE:
        quit = 1;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
 
}

void use_vsync(int vsync) { /* I dunno */ }

void set_antialiasing_samples(int samples) { /* I dunno */ }
