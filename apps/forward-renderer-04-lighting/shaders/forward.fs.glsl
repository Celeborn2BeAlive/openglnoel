#version 330

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
in vec2 vTexCoords;

out vec3 fColor;

uniform vec3 uDirectionalLightDir;
uniform vec3 uDirectionalLightIntensity;

uniform vec3 uPointLightPosition;
uniform vec3 uPointLightIntensity;

uniform vec3 uKd;

void main()
{
    float distToPointLight = length(uPointLightPosition - vViewSpacePosition);
    vec3 dirToPointLight = (uPointLightPosition - vViewSpacePosition) / distToPointLight;
    fColor = uKd * (uDirectionalLightIntensity * max(0.f, dot(vViewSpaceNormal, uDirectionalLightDir)) + uPointLightIntensity * max(0., dot(vViewSpaceNormal, dirToPointLight)) / (distToPointLight * distToPointLight));
}