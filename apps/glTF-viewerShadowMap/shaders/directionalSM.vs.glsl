#version 330

layout(location = 0) in vec3 aPosition;
uniform mat4 uDirLightViewProjMatrix;

void main()
{
    gl_Position =  uDirLightViewProjMatrix * vec4(aPosition, 1);
}