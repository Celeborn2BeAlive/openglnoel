#version 330

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
in vec2 vTexCoords;

out vec3 fColor;

uniform vec3 uDirectionalLightDir;
uniform vec3 uDirectionalLightIntensity;

uniform vec3 uPointLightPosition;
uniform vec3 uPointLightIntensity;

uniform vec3 uKa;
uniform vec3 uKd;
uniform vec3 uKs;
uniform float uShininess;

uniform sampler2D uKaSampler;
uniform sampler2D uKdSampler;
uniform sampler2D uKsSampler;
uniform sampler2D uShininessSampler;

void main()
{
    vec3 ka = uKa * vec3(texture(uKaSampler, vTexCoords));
    vec3 kd = uKd * vec3(texture(uKdSampler, vTexCoords));
    vec3 ks = uKs * vec3(texture(uKsSampler, vTexCoords));
    float shininess = uShininess * vec3(texture(uShininessSampler, vTexCoords)).x;

    vec3 normal = normalize(vViewSpaceNormal);
    vec3 eyeDir = normalize(-vViewSpacePosition);

    float distToPointLight = length(uPointLightPosition - vViewSpacePosition);
    vec3 dirToPointLight = (uPointLightPosition - vViewSpacePosition) / distToPointLight;
    vec3 pointLightIncidentLight = uPointLightIntensity / (distToPointLight * distToPointLight);

    // half vectors, for blinn-phong shading
    vec3 hPointLight = normalize(eyeDir + dirToPointLight);
    vec3 hDirLight = normalize(eyeDir + uDirectionalLightDir);

    float dothPointLight = shininess == 0 ? 1.f : max(0.f, dot(normal, hPointLight));
    float dothDirLight = shininess == 0 ? 1.f :max(0.f, dot(normal, hDirLight));

    if (shininess != 1.f && shininess != 0.f)
    {
        dothPointLight = pow(dothPointLight, shininess);
        dothDirLight = pow(dothDirLight, shininess);
    }

    fColor = ka;
    fColor += kd * (uDirectionalLightIntensity * max(0.f, dot(normal, uDirectionalLightDir)) + pointLightIncidentLight * max(0., dot(normal, dirToPointLight)));
    fColor += ks * (uDirectionalLightIntensity * dothDirLight + pointLightIncidentLight * dothPointLight);
}