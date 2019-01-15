#version 330

layout(location = 4) in vec2 aPosition;
layout(location = 2) in vec3 aColor;

out vec3 vColor;

void main()
{
    vColor = aColor;
    gl_Position = vec4(aPosition, 0, 1);
}