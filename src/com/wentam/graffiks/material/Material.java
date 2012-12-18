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

package com.wentam.graffiks.material;

import android.opengl.GLES20;

import android.content.Context;
import android.content.res.AssetManager;

import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.BufferedReader;

import com.badlogic.gdx.backends.android.AndroidGL20;

import android.util.Log;

public class Material {
    private String vertexShaderCode;
    private String fragmentShaderCode;
    private int program;
    private Context context;

    private float[] diffuseColor = {1f,1f,1f,1f};
    private float[] specularColor = {1f,1f,1f,1f};

    private float diffuseIntensity = 100f;
    private float specularIntensity = 100f;

    public String name; // this exists to make the job easier for parsers. parsers can use it for anything they want.

    private String readShaderFile(String file) {
	AssetManager assetManager = context.getAssets();
	ByteArrayOutputStream outputStream = null;
	InputStream inputStream = null;

	try {
	    inputStream = assetManager.open(file);
	    outputStream = new ByteArrayOutputStream();
	    byte buf[] = new byte[1024];
	    int len;
	    try {
		while ((len = inputStream.read(buf)) != -1) {
		    outputStream.write(buf, 0, len);
		}
		outputStream.close();
		inputStream.close();
	    } catch (IOException e) {
		Log.e("GRAFFIKS",""+e);
	    }
	} catch (IOException e) {
	    Log.e("GRAFFIKS",""+e);
	}

	return outputStream.toString();
    }

    protected void loadVertexShader(String file) {
	vertexShaderCode = readShaderFile("shaders/"+file);
    }

    protected void loadFragmentShader(String file) {
	fragmentShaderCode = readShaderFile("shaders/"+file);
    }

    protected void createVertexShader() {
	loadVertexShader("Material.vert");
    }

    protected void createFragmentShader() {
	loadFragmentShader("Material.frag");
    }

    public Material(Context cxt) {
	context = cxt;
	createVertexShader();
	createFragmentShader();
	
	AndroidGL20 gl2 = new AndroidGL20();

	// compile vertex shader
	int vertexShader = GLES20.glCreateShader(GLES20.GL_VERTEX_SHADER);
	GLES20.glShaderSource(vertexShader, vertexShaderCode);
	GLES20.glCompileShader(vertexShader);

	final int[] compileStatus = new int[1];
	GLES20.glGetShaderiv(vertexShader, GLES20.GL_COMPILE_STATUS, compileStatus, 0);

	if (compileStatus[0] == 0) {
	    Log.e("GRAFFIKS", "Error compiling vertex shader: " + gl2.glGetShaderInfoLog(vertexShader));
	    GLES20.glDeleteShader(vertexShader);
	    vertexShader = 0;
	}

	// compile fragment shader
	int fragmentShader = GLES20.glCreateShader(GLES20.GL_FRAGMENT_SHADER);
	GLES20.glShaderSource(fragmentShader, fragmentShaderCode);
	GLES20.glCompileShader(fragmentShader);

	GLES20.glGetShaderiv(fragmentShader, GLES20.GL_COMPILE_STATUS, compileStatus, 0);

	if (compileStatus[0] == 0) {
	    Log.e("GRAFFIKS", "Error compiling fragment shader: " + gl2.glGetShaderInfoLog(fragmentShader));
	    GLES20.glDeleteShader(fragmentShader);
	    fragmentShader = 0;
	}

	// link shaders together to form GLSL program
	program = GLES20.glCreateProgram();
	GLES20.glAttachShader(program, vertexShader);
	GLES20.glAttachShader(program, fragmentShader);
	GLES20.glLinkProgram(program);
    }    

    public int getProgram() {
	return program;
    }

    public void setDiffuseColor(float[] d) {
	diffuseColor = d;
    }

    public void setSpecularColor(float[] s) {
	specularColor = s;
    }

    public void setDiffuseIntensity(float i) {
	diffuseIntensity = i;
    }

    public void setSpecularIntensity(float i) {
	specularIntensity = i;
    }

    public float[] getDiffuseColor() {
	return diffuseColor;
    }

    public float[] getSpecularColor() {
	return specularColor;
    }

    public float getDiffuseIntensity() {
	return diffuseIntensity;
    }

    public float getSpecularIntensity() {
	return specularIntensity;
    }
}