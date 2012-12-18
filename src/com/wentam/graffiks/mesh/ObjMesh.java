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

import com.wentam.graffiks.GraffiksRenderer;

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




public class ObjMesh extends Mesh {
    private Context context;
    private String file;

    public ObjMesh(GraffiksRenderer r, String file) {
	super(r);
	Log.e("GRAFFIKS","constructor");
	this.file = file;	
	this.context = this.renderer.context;
	buildMesh();
    }

    private float[][] verts;
    private int[][][] faces;
    private float[][] normals;


    @Override
    protected void createMesh () {	
	parseFile(file);
	setVertices(verts);
	setFaces(faces);
	setNormals(normals);
    }

    ArrayList<String> lines;
    int vertexCount = 0;
    int normalCount = 0;
    int faceCount = 0;

    
    private void parseFile(String file) {
	long starttime = System.currentTimeMillis();
	Log.i("GRAFFIKS","parsing...");

	// we slurp the file into memory as it makes parsing much faster.
	// it's difficult to match this, so large obj files can cause problems
	// large obj files are rare in the game world luckily (well, ok, large enough to not store in memory. shush.)
	slurpFileLines(file);

	String line;

	// allocate data at correct size
	verts = new float[vertexCount][3];
	faces = new int[faceCount][3][3];
	normals = new float[normalCount][3];

	// parse the file
	int currentVertCount = 0;
	int currentNormalCount = 0;
	int currentFaceCount = 0;

	String[] lineSplit;
	String[] x;
	String[] y;
	String[] z;
	final int[] intx = new int[3];
	final int[] inty = new int[3];
	final int[] intz = new int[3];

	final int[][] face = new int[3][3];

	Pattern p = Pattern.compile(" "); // creating pattern once to improve speed
	Pattern p2 = Pattern.compile("/"); 

	int size = lines.size(); // performance again.
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

	    	faces[currentFaceCount] = face.clone();

	    	currentFaceCount++;
	    }
	}

	Log.i("GRAFFIKS","parsing took "+(System.currentTimeMillis()-starttime)+"ms");
    }


    // ppppppptthhkhkhkhkhhouuuuuup
    private void slurpFileLines(String file) {
	AssetManager assetManager = context.getAssets();
	InputStream inputStream = null;

	lines = new ArrayList<String>();
	String line = null;
	try {
	    inputStream = assetManager.open("models/"+file);
	    BufferedReader br = new BufferedReader(new InputStreamReader(inputStream, "UTF-8"));
	    
	    // loop over lines in file
	    String[] lineSplit;

	    while((line = br.readLine()) != null) {
		lines.add(line);

		if (line.indexOf("vn") == 0) {
		    normalCount++;
		} else if (line.indexOf("v") == 0) {
		    vertexCount++;
		} else if (line.indexOf("f") == 0) {
		    faceCount++;
		}
	    }

	    br.close();
	} catch (IOException e) {
	    Log.e("GRAFFIKS","Error reading file: "+e);
	}
    }
}