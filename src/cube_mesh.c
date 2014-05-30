#include "cube_mesh.h"

mesh* create_cube(float scale) {
    float verts[8][3] = {
        {scale, -scale, -scale},
        {scale, -scale, scale},
        {-scale, -scale, scale},
        {-scale, -scale, -scale},

        {scale, scale, -scale},
        {scale, scale, scale},
        {-scale, scale, scale},
        {-scale, scale, -scale}
    };

    int faces[12][3][3] = {
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

    float normals[6][3] = {
        {0, -1, 0},
        {0, 1, 0},
        {1, 0, 0},
        {0, 0, 1},
        {-1, 0, 0},
        {0, 0, -1}
    };

    mesh *m = create_mesh_st(verts,faces,12,normals);
    return m;
}
