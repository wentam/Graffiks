#include "plane_mesh.h"

mesh* create_plane(float scale) {
    float verts[4][3] = {
        {scale, -scale, 0},
        {-scale, -scale, 0},
        {scale, scale, 0},
        {-scale, scale, 0},
    };

    int faces[2][3][3] = {
        //v,vt,vn   v,vt,vn  v,vt,vn
        {{1,0,0},  {0,0,0}, {2,0,0}},
        {{3,0,0},  {1,0,0}, {2,0,0}}
    };

    float normals[1][3] = {
        {0, 0, 1}
    };

    mesh *m = create_mesh_st(verts,faces,2,normals);
    return m;
}