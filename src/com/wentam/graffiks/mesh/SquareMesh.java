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

public class SquareMesh extends Mesh {
    public SquareMesh(GraffiksRenderer renderer) {
	super(renderer);
    }

    private static float verts[] = {
	-1f,  1f, 0f,    // top left
	-1f, -1f, 0f,    // bottom left
	1f, -1f, 0f,     // bottom right
	1f, 1f, 0f       // top right
    };
   
    private static int[] faces = {
	0,1,2,
	0,2,3
    };
    
    private static float[] normals = {												
	// face 1
	0f, 0f, -1f,
  
	// face 2
	0f, 0f, -1f
    };

    @Override
    protected void createMesh () {
	// setVertices(verts);
	// setFaces(faces);
	// setNormals(normals);
    }
}