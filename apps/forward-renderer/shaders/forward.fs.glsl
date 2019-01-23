#version 330

in vec3 vPosition_vs;
in vec3 vNormal_vs; 
in vec2 vTexCoords;

out vec3 fColor;

uniform vec3 uDirectionalLightDir;
uniform vec3 uDirectionalLightIntensity;

uniform vec3 uPointLightPosition;
uniform vec3 uPointLightIntensity;

uniform vec3 uKd;

uniform sampler2D uKdSampler;

void main() {

	float distToPointLight = length(uPointLightPosition - vPosition_vs);
    	vec3 dirToPointLight = (uPointLightPosition - vPosition_vs) / distToPointLight;
    	vec3 kd = uKd * vec3(texture(uKdSampler, vTexCoords));
	fColor = kd * (uDirectionalLightIntensity * max(0.f, dot(vNormal_vs, uDirectionalLightDir)) + uPointLightIntensity * max(0., dot(vNormal_vs, dirToPointLight)) / (distToPointLight * distToPointLight));

	//fColor = vec3(vTexCoords, 0);
}

