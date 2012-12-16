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

public class TriangleMesh extends Mesh {
    public TriangleMesh(GraffiksRenderer renderer) {
	super(renderer);
    }

    private static float verts[] = {
	0f,  1f, 0f,   // top
	-1f, -1f, 0f,   // bottom left
	1f, -1f, 0f    // bottom right
    };

    @Override
    protected void createMesh () {
	// setVertices(verts);
    }
}

				  