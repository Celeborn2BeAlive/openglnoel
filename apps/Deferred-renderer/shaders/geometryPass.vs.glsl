
#version 330

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
//layout (location = 3) in vec3 Tangent;

out vec3 vViewSpacePosition;
out vec3 vViewSpaceNormal;
out vec2 vTexCoords;

uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uNormalMatrix;

void main()
{
    vViewSpacePosition = vec3(uModelViewMatrix * vec4(aPosition, 1));
	vViewSpaceNormal = vec3(uNormalMatrix * vec4(aNormal, 0));
	vTexCoords = aTexCoords;
    gl_Position =  uModelViewProjMatrix * vec4(aPosition, 1);
}