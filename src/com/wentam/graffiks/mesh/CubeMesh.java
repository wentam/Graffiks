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

public class CubeMesh extends Mesh {
    public CubeMesh(GraffiksRenderer renderer) {
	super(renderer);
    }

    private static float verts[][] = {
	{1f, -1f, -1f},
	{1f, -1f, 1f},
	{-1f, -1f, 1f},
	{-1f, -1f, -1f},

	{1f, 1f, -1f},
	{1f, 1f, 1f},
	{-1f, 1f, 1f},
	{-1f, 1f, -1f}
    };

    private static int[][][] faces = {
	//v,vt,vn   v,vt,vn  v,vt,vn
	{{0,0,0},  {1,1,0}, {2,2,0} },
	{{0,0,0},  {2,2,0}, {3,3,0} },
	{{4,4,1},  {7,5,1}, {6,6,1} },
	{{4,4,1},  {6,6,1}, {5,7,1} },
	{{0,8,2},  {4,4,2}, {5,7,2} },
	{{0,8,2},  {5,7,2}, {1,9,2} },
	{{1,10,3}, {5,7,3}, {6,6,3} },
	{{1,10,3}, {6,6,3}, {2,11,3}},
        {{2,12,4}, {6,6,4}, {7,5,4} },
	{{2,12,4}, {7,5,4}, {3,13,4}},
	{{4,4,5},  {0,0,5}, {3,3,5} },
	{{4,4,5},  {3,3,5}, {7,5,5} }
    };

    private static float[][] normals =
    {												
	{0.000000f, -1.000000f, 0.000000f},
	{0.000000f, 1.000000f, 0.000000f},
	{1.000000f, 0.000000f, 0.000000f},
	{0.000000f, 0.000000f, 1.000000f},
	{-1.000000f, 0.000000f, 0.000000f},
       	{0.000000f, 0.000000f, -1.000000f}
    };

    @Override
    protected void createMesh () {
	setVertices(verts);
	setFaces(faces);
	setNormals(normals);
    }
}