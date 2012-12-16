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

import android.opengl.GLSurfaceView;
import android.content.Context;

import android.util.Log;

import com.wentam.graffiks.material.*;
import com.wentam.graffiks.mesh.*;
import com.wentam.graffiks.GraffiksRenderer;

public abstract class GraffiksView extends GLSurfaceView {
    protected Context context;

    public GraffiksView(Context c) {
	super(c);
	context = c;       	

	setEGLContextClientVersion(2);
	// setEGLConfigChooser(8, 8, 8, 8, 16, 0);
	setRenderer(new GraffiksRenderer(context, this));
    }

    public abstract void init(GraffiksRenderer renderer);
    public abstract void update(float timeStep);
    public abstract void draw();
}