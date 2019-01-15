#version 330

layout(location = 0) in vec3 aPosition;

void main()
{
    // The vertex shader does nothing, transformations are performed in the geometry shader
    gl_Position = vec4(aPosition, 1);
}