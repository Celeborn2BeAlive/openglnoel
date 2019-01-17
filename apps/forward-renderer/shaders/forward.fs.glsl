#version 330

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
in vec2 vTexCoords;

 uniform vec3 uDirectionalLightDir;
 uniform vec3 uDirectionalLightIntensity;

 uniform vec3 uPointLightPosition;
 uniform vec3 uPointLightIntensity;

 uniform vec3 uKd;

out vec3 fColor;

void main()
{
   fColor = vec3(vTexCoords, 0);
}