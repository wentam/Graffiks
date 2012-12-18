//////////////////////////////////////////////////////////////////////////////
// Copyright 2012 Matthew Egeler
// 									       
// Licensed under the Apache License, Version 2.0 (the "License");	       
// you may not use this file except in compliance with the License.	       
// You may obtain a copy of the License at				      
// 									       
//     http://www.apache.org/licenses/LICENSE-2.0			       
// 									       
// Unless required by applicable law or agreed to in writing, software      
// distributed under the License is distributed on an "AS IS" BASIS,	       
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and      
// limitations under the License.					      
//////////////////////////////////////////////////////////////////////////////

package com.wentam.graffiks;

import javax.microedition.khronos.opengles.GL10; // unused, but we need it as android will always pass us GL10.
import javax.microedition.khronos.egl.EGLConfig;

import android.opengl.GLSurfaceView;
import android.opengl.GLSurfaceView.Renderer;
import android.opengl.GLES20;
import android.opengl.Matrix;

import android.content.Context;

import android.util.Log;

import java.lang.Thread;
import java.lang.Exception;

import com.wentam.graffiks.material.*;
import com.wentam.graffiks.mesh.*;
import com.wentam.graffiks.GraffiksView;

import android.opengl.GLU;

import android.os.SystemClock;

public class GraffiksRenderer implements GLSurfaceView.Renderer {

    public float projectionMatrix[] = new float[16];
    public float viewMatrix[] = new float[16];
    public Context context;
    private GraffiksView view;

    // vars declared here to avoid GCs
    private long startTime;
    private long endTime;
    private long dt;   
    private int ms_per_frame;

    private float ambientColor[] = {0f,0f,0f,1f};

    public GraffiksRenderer(Context cntxt, GraffiksView v) {
	super();
	context = cntxt;
	view = v;
    }
   
    public void onSurfaceCreated (GL10 unused, EGLConfig config) {
	GLES20.glClearColor(0f,0f,0f,1.0f);

	GLES20.glEnable(GLES20.GL_CULL_FACE);
 	GLES20.glEnable(GLES20.GL_DEPTH_TEST);
	GLES20.glDepthFunc(GLES20.GL_LESS);
	GLES20.glDepthMask(true);

	view.init(this);
	startTime = System.currentTimeMillis();
    }

    float smoothedDeltaRealTime_ms=16.6f;
    float movAverageDeltaTime_ms=smoothedDeltaRealTime_ms;
    long lastRealTimeMeasurement_ms;

    static final float movAveragePeriod=60;
    static final float smoothFactor=0.1f;

    public void onDrawFrame(GL10 unused) {
	// limit FPS so we aren't wasting battery on the user's device as it renders 100s of FPS...
	// most screens won't display more than 60FPS, so we'll use that.
	limitFPS(60);

	// update
	view.update(smoothedDeltaRealTime_ms);

	// draw
	GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);
	Matrix.setLookAtM(viewMatrix, 0, 0, 0, -20, 0f, 0f, 0f, 0f, 1.0f, 0.0f);

	view.draw();


	// we use a smoothed moving average to handle delta time,
	// this way we get less janks, as well as the lack of a sudden jolt to catch up.

	// moving average
        long currTimePick_ms=SystemClock.uptimeMillis();
        float realTimeElapsed_ms;
        if (lastRealTimeMeasurement_ms>0){
        realTimeElapsed_ms=(currTimePick_ms - lastRealTimeMeasurement_ms);
        } else {
                 realTimeElapsed_ms=smoothedDeltaRealTime_ms; // just the first time
        }
        movAverageDeltaTime_ms=(realTimeElapsed_ms + movAverageDeltaTime_ms*(movAveragePeriod-1))/movAveragePeriod;

	// smoothed average
        smoothedDeltaRealTime_ms=smoothedDeltaRealTime_ms +(movAverageDeltaTime_ms - smoothedDeltaRealTime_ms)* smoothFactor;

        lastRealTimeMeasurement_ms=currTimePick_ms;
    }

    public void onSurfaceChanged(GL10 unused, int width, int height) {
	GLES20.glViewport(0, 0, width, height);

	float ratio = (float) width / height;
	float ratioReversed = (float) height / width;
	Matrix.frustumM(projectionMatrix, 0, -ratio, ratio, -1f, 1f, 5, 100);
    }

    public void setAmbientLightColor(float color[]) {
	ambientColor = color;
    }

    public float[] getAmbientLightColor() {
	return ambientColor;
    }

    private void limitFPS(int FPS) {	
	ms_per_frame = 1000/FPS;
	
	endTime = System.currentTimeMillis();
	dt = endTime - startTime;
	if (dt < ms_per_frame) {
	    try {
		Thread.sleep(ms_per_frame - dt);
	    } catch (Exception e) {}
	}
	startTime = System.currentTimeMillis();
    }
}