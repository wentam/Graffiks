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

package com.wentam.graffiks.loader;

import com.wentam.graffiks.GraffiksRenderer;
import com.wentam.graffiks.mesh.Mesh;
import com.wentam.graffiks.material.Material;
import com.wentam.graffiks.obj.Obj;

import android.content.Context;
import android.content.res.AssetManager;

import android.util.Log;

import java.util.ArrayList;
import java.util.regex.Pattern;
import java.util.StringTokenizer;

import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.io.IOException;

public class ObjLoader extends Loader {
    private Context context;
    private String file;
    private GraffiksRenderer renderer;

    // lines
    private ArrayList<String> lines;
    private ArrayList<String> MtlLines;

    // counts 
    private int vertexCount = 0;
    private int normalCount = 0;
    private ArrayList<Integer> faceCount = new ArrayList<Integer>();
    private int materialCount = 0;
    private int uniqueMaterialCount = 0;

    // target format        
    private ArrayList<int[][][]> faces = new ArrayList<int[][][]>();
    private float[][] verts;
    private float[][] normals;
    private String[] material_names;
    private Material[] materials;

    // counts
    private int objectCount = 0;

    // final meshes
    private Mesh[] meshes;

    // final obj
    private Obj obj;

    public ObjLoader(GraffiksRenderer renderer) {
	super();
	this.renderer = renderer;
	this.file = file;
	this.context = this.renderer.context;
    }

    public Obj load(String file, String mtl) {
	parseFile(file);
	parseMtlFile(mtl);

	meshes = new Mesh[faces.size()];

	obj = new Obj(renderer);

	// create array of indicies that refer to the correct materials in the correct order based on material_names
	int[] materialIndicies = new int[material_names.length];

	long starttime = System.currentTimeMillis();
	for (int i = 0; i < material_names.length; i++) {
	    for (int i2 = 0; i2 < materials.length; i2++) {
		if (materials[i2].name.equals(material_names[i])) {
		    materialIndicies[i] = i2;
		    break;
		}
	    }
	}

	starttime = System.currentTimeMillis();
	Log.i("GRAFFIKS","creating obj");
	for (int i = 0; i < meshes.length; i++) {	   
	    meshes[i] = new Mesh(renderer);
	    meshes[i].setVertices(verts);
	    meshes[i].setNormals(normals);
	    meshes[i].setFaces(faces.get(i));
	    meshes[i].initMesh();
	    obj.addMesh(meshes[i]);
	    obj.addMaterial(materials[materialIndicies[i]]);
	}
	Log.i("GRAFFIKS","obj creation took "+(System.currentTimeMillis()-starttime)+"ms");

	return obj;
    }

    private void parseMtlFile(String file) {
	// read file
	slurpMtlLines(file);

	// parse file
	materials = new Material[uniqueMaterialCount];

	int size = MtlLines.size();
	String[] lineSplit;

	int currentMaterialIndex = -1;
	String currentMaterialName = "";
	float[] currentDiffuseColor = {0f,0f,0f,1f};

	Pattern p = Pattern.compile(" "); 

	for(int i = 0; i < size; i++) {
	    lineSplit = p.split(MtlLines.get(i));
	    
	    if (lineSplit[0].equals("newmtl")) { 
		// new material. save the previous data and move on.

		if (currentMaterialIndex >= 0) { // only save previous data if it exists
		    materials[currentMaterialIndex] = new Material(this.context);
		    materials[currentMaterialIndex].setDiffuseColor(currentDiffuseColor.clone());
		    materials[currentMaterialIndex].name = currentMaterialName;
		}


		currentMaterialIndex++;
		currentMaterialName = lineSplit[1];
	    } else if (lineSplit[0].equals("Kd")) {
		currentDiffuseColor[0] = Float.parseFloat(lineSplit[1]);
		currentDiffuseColor[1] = Float.parseFloat(lineSplit[2]);
		currentDiffuseColor[2] = Float.parseFloat(lineSplit[3]);
	    }
	}

	// we missed the last material, save it here.
	materials[currentMaterialIndex] = new Material(this.context);
	materials[currentMaterialIndex].setDiffuseColor(currentDiffuseColor.clone());
	materials[currentMaterialIndex].name = currentMaterialName;
    }

    private void parseFile(String file) {
	long starttime = System.currentTimeMillis();
	Log.i("GRAFFIKS","parsing...");

	// we slurp the file into memory as it makes parsing much faster.
	slurpFileLines(file);

	// allocate data at correct size
	verts = new float[vertexCount][3];
	normals = new float[normalCount][3];
	material_names = new String[materialCount];

	// parse the file
	int currentVertCount = 0;
	int currentNormalCount = 0;
	int currentFaceCount = 0;

	int currentFacesIndex = -1;
	
	String[] lineSplit;
	String[] x;
	String[] y;
	String[] z;

	final int[] intx = new int[3];
	final int[] inty = new int[3];
	final int[] intz = new int[3];
	final int[][] face = new int[3][3];

	Pattern p = Pattern.compile(" ");  // creating regex patterns once to improve speed
	Pattern p2 = Pattern.compile("/"); //

	int size = lines.size();

	int[][][] tmpfaces = null;

	for (int i = 0; i < size; i++) {
	    lineSplit = p.split(lines.get(i));

	    if (lineSplit[0].equals("v")) {
	    	verts[currentVertCount][0] = Float.parseFloat(lineSplit[1]);
	    	verts[currentVertCount][1] = Float.parseFloat(lineSplit[2]);
	    	verts[currentVertCount][2] = Float.parseFloat(lineSplit[3]);
	    	currentVertCount++;	    
	    } else if (lineSplit[0].equals("vn")) {
	    	normals[currentNormalCount][0] = Float.parseFloat(lineSplit[1]);
	    	normals[currentNormalCount][1] = Float.parseFloat(lineSplit[2]);
	    	normals[currentNormalCount][2] = Float.parseFloat(lineSplit[3]);
	    	currentNormalCount++;
	    } else if (lineSplit[0].equals("usemtl")) {
		if (currentFacesIndex >= 0) {
		    faces.add(tmpfaces);
		}

		currentFacesIndex++;

		material_names[currentFacesIndex] = lineSplit[1];
		tmpfaces = new int[faceCount.get(currentFacesIndex)][3][3];
				currentFaceCount = 0;		

	    } else if (lineSplit[0].equals("f")) {

	    	// split
	    	x = p2.split(lineSplit[1]);
	    	y = p2.split(lineSplit[2]);
	    	z = p2.split(lineSplit[3]);

	    	// convert to ints
	    	intx[0] = Integer.parseInt(x[0])-1;
	    	intx[1] = Integer.parseInt(x[1])-1;
	    	intx[2] = Integer.parseInt(x[2])-1;

	    	inty[0] = Integer.parseInt(y[0])-1;
	    	inty[1] = Integer.parseInt(y[1])-1;
	    	inty[2] = Integer.parseInt(y[2])-1;

	    	intz[0] = Integer.parseInt(z[0])-1;
	    	intz[1] = Integer.parseInt(z[1])-1;
	    	intz[2] = Integer.parseInt(z[2])-1;

	    	// create face
	    	face[0] = intx.clone();
	    	face[1] = inty.clone();
	    	face[2] = intz.clone();

		tmpfaces[currentFaceCount] = face.clone();

	    	currentFaceCount++;
	    }
	}

	faces.add(tmpfaces);

	Log.i("GRAFFIKS","parsing took "+(System.currentTimeMillis()-starttime)+"ms");
    }

    
    private void slurpMtlLines(String file) {
	AssetManager assetManager = context.getAssets();
	InputStream inputStream = null;

	MtlLines = new ArrayList<String>();
	String line = null;

	try {
	    inputStream = assetManager.open("models/"+file);
	    BufferedReader br = new BufferedReader(new InputStreamReader(inputStream, "UTF-8"));
	    
	    // loop over lines in file
	    while((line = br.readLine()) != null) {
		MtlLines.add(line);

		if (line.indexOf("newmtl") == 0) {
		    uniqueMaterialCount++;
		}
	    }

	    br.close();
	} catch (IOException e) {
	    Log.e("GRAFFIKS","Error reading file: "+e);
	}
    }
    

    private void slurpFileLines(String file) {
	AssetManager assetManager = context.getAssets();
	InputStream inputStream = null;

	lines = new ArrayList<String>();
	String line = null;
	try {
	    inputStream = assetManager.open("models/"+file);
	    BufferedReader br = new BufferedReader(new InputStreamReader(inputStream, "UTF-8"));
	    
	    int currentMesh = 0;
	    int currentMeshNormalCount = 0;
	    int currentMeshVertexCount = 0;
	    int currentMeshFaceCount = 0;
	    int currentFaceIndex = 0;

	    // loop over lines in file
	    while((line = br.readLine()) != null) {
		lines.add(line);

		if (line.indexOf("vn") == 0) {   // !important, which is why it's first. (vn collides with v)
		    normalCount++;
		} else if (line.indexOf("v") == 0) {
		    vertexCount++;
		} else if (line.indexOf("f") == 0) {
		    faceCount.set(currentFaceIndex-1, faceCount.get(currentFaceIndex-1)+1); // increment
		} else if (line.indexOf("o") == 0) {
		    objectCount++;
		} else if (line.indexOf("usemtl") == 0) {
		    materialCount++;
		    currentFaceIndex++;
		    faceCount.add(0);
		}
	    }

	    br.close();
	} catch (IOException e) {
	    Log.e("GRAFFIKS","Error reading file: "+e);
	}
    }
}
