/*************************************************************************
 *
 *   fbCube.geom
 *
 *   Created by dmitry
 *   22.04.2021
 *
 ***/

#version 460 core


// input primitive type - triangle
layout (triangles) in;
// output primitive type
layout (triangle_strip, max_vertices=18) out;


// six matrices define view matrices from the
// center of the cube to each of its faces
layout(set = 1, binding = 0) uniform mat4 shadowMatrices[6];


void main()
{
    for(int face = 0; face < 6; ++face)
    {
        // built-in variable defining which
        // the face of the cube map is being rendered
        gl_Layer = face;

        // loop over all vertices of the triangle
        for(int i = 0; i < 3; ++i)
        {
            gl_Position = shadowMatrices[face] * gl_in[i].gl_Position;
            // fix the vertex. gl_position for each vertex has its own
            EmitVertex();
        }
        EndPrimitive();
    }
}