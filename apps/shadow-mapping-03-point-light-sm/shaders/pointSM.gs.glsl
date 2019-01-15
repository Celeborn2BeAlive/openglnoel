#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 uPointLightViewProjMatrix[6];

void main() {
    // Create 6 triangles, each one projected to a face of the cube map
    for(int face = 0; face < 6; ++face) {
        gl_Layer = face;
        for(int i = 0; i < 3; ++i) {
            gl_Position = uPointLightViewProjMatrix[face] * gl_in[i].gl_Position;
            EmitVertex();
        }
        EndPrimitive();
    }
}