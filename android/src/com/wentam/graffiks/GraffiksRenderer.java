package com.wentam.graffiks;

import javax.microedition.khronos.opengles.GL10; // unused, but we need it as android will always pass us GL10.
import javax.microedition.khronos.egl.EGLConfig;

import android.opengl.GLSurfaceView;
import android.opengl.GLSurfaceView.Renderer;
import android.opengl.GLES20;

import android.content.Context;

import android.util.Log;

import java.lang.Thread;
import java.lang.Exception;

import com.wentam.graffiks.GraffiksView;

import android.os.SystemClock;

import android.content.res.AssetManager;

public class GraffiksRenderer implements GLSurfaceView.Renderer {
    public Context context;
    private GraffiksView view;

    public GraffiksRenderer(Context context, GraffiksView v) {
        super();
        this.context = context;
        view = v;
    }

    static {
        System.loadLibrary("graffiks");
    }

    public static native void on_surface_created(AssetManager assetManager);
    public static native void on_surface_changed(int width, int height);
    public static native void on_draw_frame();
    public static native void on_finish();

    private AssetManager mgr;

    public void onSurfaceCreated (GL10 unused, EGLConfig config) {
        mgr = context.getResources().getAssets();
        on_surface_created(mgr);
    }

    public void onDrawFrame(GL10 unused) {
        on_draw_frame();
    }

    public void onSurfaceChanged(GL10 unused, int width, int height) {
        on_surface_changed(width, height);
    }

    public void finish() {
        on_finish();
    }
}
