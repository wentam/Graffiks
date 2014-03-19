package com.wentam.graffiks;

import android.opengl.GLSurfaceView;
import android.content.Context;

import android.util.Log;

import com.wentam.graffiks.GraffiksRenderer;

public class GraffiksView extends GLSurfaceView {
    protected Context context;
    private GraffiksRenderer renderer;

    public GraffiksView(Context c) {
        super(c);
        context = c;

        setEGLContextClientVersion(2);
        // setEGLConfigChooser(8, 8, 8, 8, 16, 0);

        renderer = new GraffiksRenderer(context,this);
        setRenderer(renderer);
    }

    protected void onDetachedFromWindow() {
        renderer.finish();
    }
}
