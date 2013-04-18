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

package com.wentam.graffiks.mesh;

import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import android.opengl.GLES20;
import android.opengl.Matrix;

import android.util.Log;

import com.wentam.graffiks.material.Material;
import com.wentam.graffiks.GraffiksRenderer;

import java.util.ArrayList;

public class Mesh {
    private static final int coordsPerVertex = 3;
    private static final int coordsPerNormal = 3;

    private FloatBuffer vertexBuffer;
    private FloatBuffer normalBuffer;
    private int vertexCount;
    private int faceCount;

    // mesh data
    private float[][] complexVertices;
    private int[][][] complexFaces;
    private float[][] complexNormals;

    // we convert to the following opengl format for mesh data
    private float[] vertices;
    private int[] indicies;
    private float[] normals;

    // rotation
    private float rotationMatrix[] = new float[16];
    private float angleX = 0f;
    private float angleY = 0f;
    private float angleZ = 0f;
    private float rotationCenterX = 0f;
    private float rotationCenterY = 0f;
    private float rotationCenterZ = 0f;
    private float angle = 0f;         // this is opengl's representation of rotation, which is what we convert to
    private float rotationAxisX = 1f; //
    private float rotationAxisY = 0f; //
    private float rotationAxisZ = 0f; //

    // location
    private float locationMatrix[] = new float[16];
    private float locationX = 0f;
    private float locationY = 0f;
    private float locationZ = 0f;
    private float prevLocationX = 0f;
    private float prevLocationY = 0f;
    private float prevLocationZ = 0f;

    // matrices
    private float projectionModelViewMatrix[] = new float[16];
    private float modelViewMatrix[] = new float[16];
    private float modelMatrix[] = new float[16];
    private float modelRotationMatrix[] = new float[16];


    // renderer
    protected GraffiksRenderer renderer;

    public Mesh(GraffiksRenderer r) {
	renderer = r;
    }

    protected void buildMesh() {
	createMesh();
	initMesh();
    }

    // Override this to create your mesh
    protected void createMesh (){

    }
    
    // stuff to avoid GC's
    Float[][] vertexPosition;
    Float[][] vertexNormal;
    int current_vertex_index;
    int current_normal_index;

    int i;
    int i2;
    int i3;

    ByteBuffer bb;
    ByteBuffer bb2;
    
    public void initMesh () {
	ArrayList<Float[]>   tmp_vertices = new ArrayList<Float[]>();
	ArrayList<Integer> tmp_indicies = new ArrayList<Integer>();
	ArrayList<Float[]>   tmp_normals = new ArrayList<Float[]>();
	
	// iterate over all faces
	for (int[][] face : complexFaces) {
	    // construct array of positions and normals we want to create a vertex for.
	    // must use Float object as we're useing arraylists for calculation
	    vertexPosition = new Float[3][3];	 
	    vertexNormal = new Float[3][3];
	    
	    // vertex 1
	    vertexPosition[0][0] = new Float(complexVertices[face[0][0]][0]);
	    vertexPosition[0][1] = new Float(complexVertices[face[0][0]][1]);
	    vertexPosition[0][2] = new Float(complexVertices[face[0][0]][2]);

	    // vertex 2
	    vertexPosition[1][0] = new Float(complexVertices[face[1][0]][0]);
	    vertexPosition[1][1] = new Float(complexVertices[face[1][0]][1]);
	    vertexPosition[1][2] = new Float(complexVertices[face[1][0]][2]);

	    // vertex 3
	    vertexPosition[2][0] = new Float(complexVertices[face[2][0]][0]);
	    vertexPosition[2][1] = new Float(complexVertices[face[2][0]][1]);
	    vertexPosition[2][2] = new Float(complexVertices[face[2][0]][2]);

	    // normal 1
	    vertexNormal[0][0] = new Float(complexNormals[face[0][2]][0]);
	    vertexNormal[0][1] = new Float(complexNormals[face[0][2]][1]);
	    vertexNormal[0][2] = new Float(complexNormals[face[0][2]][2]);

	    // normal 2
	    vertexNormal[1][0] = new Float(complexNormals[face[1][2]][0]);
	    vertexNormal[1][1] = new Float(complexNormals[face[1][2]][1]);
	    vertexNormal[1][2] = new Float(complexNormals[face[1][2]][2]);

	    // normal 3
	    vertexNormal[2][0] = new Float(complexNormals[face[2][2]][0]);
	    vertexNormal[2][1] = new Float(complexNormals[face[2][2]][1]);
	    vertexNormal[2][2] = new Float(complexNormals[face[2][2]][2]);


	    // iterate over the vertices we want to create
	    for (i3 = 0; i3 < vertexPosition.length; i3++) {
		tmp_vertices.add(vertexPosition[i3]);
		tmp_normals.add(vertexNormal[i3]);
	    }	   
	}
	    
	// convert ArrayLists to normal java arrays
	
	// vertices
	vertices = new float[tmp_vertices.size()*3];
	current_vertex_index = 0;
	for (i = 0; i < tmp_vertices.size(); i++) {
	    for (i2 = 0; i2 < tmp_vertices.get(i).length; i2++ ) {
		vertices[current_vertex_index++] = tmp_vertices.get(i)[i2];
	    }
	}

	// normals
	normals = new float[tmp_normals.size()*3];
	current_normal_index = 0;
	for (i = 0; i < tmp_normals.size(); i++) {
	    for (i2 = 0; i2 < tmp_normals.get(i).length; i2++ ) {
		normals[current_normal_index++] = tmp_normals.get(i)[i2];
	    }
	}

	    

	// create buffers from the arrays
	    
	// define vertexCount
	vertexCount = vertices.length;
	    
	// create vertexBuffer 
	bb = ByteBuffer.allocateDirect(vertexCount * 4);
	bb.order(ByteOrder.nativeOrder());
	    
	vertexBuffer = bb.asFloatBuffer();
	vertexBuffer.put(vertices);
	vertexBuffer.position(0);

	// create normalBuffer
	bb2 = ByteBuffer.allocateDirect(normals.length * 4);
	bb2.order(ByteOrder.nativeOrder());
	    
	normalBuffer = bb2.asFloatBuffer();
	normalBuffer.put(normals);
	normalBuffer.position(0);
    }

    public void setVertices (float[][] v) {
	complexVertices = v;
    }

    // TODO: optionally accept faces as quads and triangulate them.
    public void setFaces (int[][][] f) {
	complexFaces = f;
    }

    public void setNormals (float[][] n) {	
	complexNormals = n;
    }
    
    // variables defined here to avoid GC's during draw
    private int program;

    private float[] ambientColor = {0f,0f,0f,1f};
    private float[] diffuseColor = {0.5f,0.5f,0.5f,1.0f};
    private float[] specularColor = {1f,1f,1f,1f};

    private float diffuseIntensity;
    private float specularIntensity;

    private float[] lightLocation = {5f,5f,5f};

    private float offsetLocationX = 0f;
    private float offsetLocationY = 0f;
    private float offsetLocationZ = 0f;

    // handles    
    private int MVPMatrixHandle;
    private int MVMatrixHandle;
    private int vertexPositionHandle;
    private int ambientHandle;
    private int diffuseHandle;
    private int specularHandle;
    private int normalHandle;
    private int lightPositionHandle;
    private int diffuseIntensityHandle;
    private int specularIntensityHandle;

    public void draw (Material material) {
	// get program from material
	program = material.getProgram();

	// use program
	GLES20.glUseProgram(program);

	// get handles
	MVPMatrixHandle = GLES20.glGetUniformLocation(program, "uMVPMatrix");
	MVMatrixHandle = GLES20.glGetUniformLocation(program, "uMVMatrix");
	ambientHandle =  GLES20.glGetUniformLocation(program, "u_ambient_color");
	diffuseHandle =  GLES20.glGetUniformLocation(program, "u_diffuse_color");
	specularHandle =  GLES20.glGetUniformLocation(program, "u_specular_color");
	specularIntensityHandle =  GLES20.glGetUniformLocation(program, "u_specular_intensity");
	diffuseIntensityHandle =  GLES20.glGetUniformLocation(program, "u_diffuse_intensity");
	lightPositionHandle =  GLES20.glGetUniformLocation(program, "u_light_position");
	vertexPositionHandle = GLES20.glGetAttribLocation(program, "a_position");
	normalHandle = GLES20.glGetAttribLocation(program, "a_normal");

	// add triangle verts
	GLES20.glEnableVertexAttribArray(vertexPositionHandle);
	GLES20.glVertexAttribPointer(vertexPositionHandle, coordsPerVertex,
				     GLES20.GL_FLOAT, false,
				     (coordsPerVertex*4), vertexBuffer);

	// add triangle normals
	GLES20.glEnableVertexAttribArray(normalHandle);
	GLES20.glVertexAttribPointer(normalHandle, coordsPerNormal,
				     GLES20.GL_FLOAT, false,
				     (coordsPerNormal*4), normalBuffer);

       	// model matrix (rotate and translate)
	
	// reset matrices
	modelMatrix = new float[16];
	modelViewMatrix = new float[16];
	projectionModelViewMatrix = new float[16];

	Matrix.setIdentityM(modelMatrix, 0);	
	
	// translate
	Matrix.translateM(modelMatrix, 0, locationX, locationY, locationZ);

	// rotate
	Matrix.rotateM(modelMatrix, 0, angle, rotationAxisX, rotationAxisY, rotationAxisZ);


  	// combine our model matrix with the view matrix
	Matrix.multiplyMM(modelViewMatrix, 0, renderer.viewMatrix, 0, modelMatrix, 0);
	

	// combine modelViewMatrix with renderer.projectionMatrix
	Matrix.multiplyMM(projectionModelViewMatrix, 0, renderer.projectionMatrix, 0, modelViewMatrix, 0);

	ambientColor = renderer.getAmbientLightColor();
	diffuseColor = material.getDiffuseColor();
	specularColor = material.getSpecularColor();

	diffuseIntensity = material.getDiffuseIntensity();
	specularIntensity = material.getSpecularIntensity();

	// TODO: pull this from...uhh, some sort of list of light objects?
	// lightLocation = {5f,5f,5f};

	// send stuff to shader
	GLES20.glUniformMatrix4fv(MVPMatrixHandle, 1, false, projectionModelViewMatrix, 0);
	GLES20.glUniformMatrix4fv(MVMatrixHandle, 1, false, modelViewMatrix, 0);
	GLES20.glUniform4f(ambientHandle, ambientColor[0], ambientColor[1], ambientColor[2], ambientColor[3]);
	GLES20.glUniform4f(diffuseHandle, diffuseColor[0], diffuseColor[1], diffuseColor[2], diffuseColor[3]);
	GLES20.glUniform4f(specularHandle, specularColor[0],specularColor[1], specularColor[2], specularColor[3]);
	GLES20.glUniform3f(lightPositionHandle, lightLocation[0], lightLocation[1], lightLocation[2]);
	GLES20.glUniform1f(diffuseIntensityHandle, diffuseIntensity);
	GLES20.glUniform1f(specularIntensityHandle, specularIntensity);

	// draw it
	GLES20.glDrawArrays(GLES20.GL_TRIANGLES, 0, vertexCount/3);

	// disable vertex array
	GLES20.glDisableVertexAttribArray(vertexPositionHandle);
	GLES20.glDisableVertexAttribArray(normalHandle);
    }

    public void setRotationCenter(float x, float y, float z) {
	//TODO: CURRENLTY DOES NOTHING
	rotationCenterX = x;
	rotationCenterY = y;
	rotationCenterZ = z;
	//TODO: CURRENLTY DOES NOTHING
    }


    public void setAngleDegrees(float x, float y, float z) {
	// set angles to object for retrieval later (this isn't related to the math)
	angleX = x;
	angleY = y;
	angleZ = z;

	// convert from degrees to radians.
	double radiansX = Math.toRadians(x);
	double radiansY = Math.toRadians(y);
	double radiansZ = Math.toRadians(z);

	// convert from euler to axis-angle
        double c1 = Math.cos(radiansX/2.0);
	double c2 = Math.cos(radiansY/2.0);
	double c3 = Math.cos(radiansZ/2.0);

	double s1 = Math.sin(radiansX/2.0);
	double s2 = Math.sin(radiansY/2.0);
	double s3 = Math.sin(radiansZ/2.0);

	angle = (float) Math.toDegrees((2f * Math.acos(c1*c2*c3 - s1*s2*s3)));
	rotationAxisX = (float) (s1*s2*c3 + c1*c2*s3);
	rotationAxisY = (float) (s1*c2*c3 + c1*s2*s3);
	rotationAxisZ = (float) (c1*s2*c3 - s1*c2*s3);

	// normalize result
	double normal = Math.sqrt((rotationAxisX*rotationAxisX)+(rotationAxisY*rotationAxisY)+(rotationAxisZ*rotationAxisZ));

	rotationAxisX /= normal;
	rotationAxisY /= normal;
	rotationAxisZ /= normal;
    }

    public void setLocation(float x, float y, float z) {
	locationX = x;
	locationY = y;
	locationZ = z;
    }

    public float getAngleDegreesX() {
	return angleX;
    }

    public float getAngleDegreesY() {
	return angleY;
    }

    public float getAngleDegreesZ() {
	return angleZ;
    }

    public float getLocationX() {
	return locationX;
    }

    public float getLocationY() {
	return locationY;
    }

    public float getLocationZ() {
	return locationZ;
    }
}