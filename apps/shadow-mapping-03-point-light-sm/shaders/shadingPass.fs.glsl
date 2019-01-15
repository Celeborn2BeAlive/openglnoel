#version 330

// Everything in View Space
uniform sampler2D uGPosition;
uniform sampler2D uGNormal;
uniform sampler2D uGAmbient;
uniform sampler2D uGDiffuse;
uniform sampler2D uGGlossyShininess;

out vec3 fColor;

uniform vec3 uDirectionalLightDir;
uniform vec3 uDirectionalLightIntensity;

uniform vec3 uPointLightPosition;
uniform vec3 uPointLightIntensity;

uniform mat4 uDirLightViewProjMatrix;
uniform sampler2DShadow uDirLightShadowMap;
uniform float uDirLightShadowMapBias;
uniform int uDirLightShadowMapSampleCount;
uniform float uDirLightShadowMapSpread;

uniform mat4 uPointLightViewMatrix;
uniform mat4 uPointLightViewProjMatrix[6];
uniform samplerCube uPointLightShadowMap;
uniform float uPointLightShadowMapBias;

vec2 poissonDisk[16] = vec2[](
    vec2( -0.94201624, -0.39906216 ),
    vec2( 0.94558609, -0.76890725 ),
    vec2( -0.094184101, -0.92938870 ),
    vec2( 0.34495938, 0.29387760 ),
    vec2( -0.91588581, 0.45771432 ),
    vec2( -0.81544232, -0.87912464 ),
    vec2( -0.38277543, 0.27676845 ),
    vec2( 0.97484398, 0.75648379 ),
    vec2( 0.44323325, -0.97511554 ),
    vec2( 0.53742981, -0.47373420 ),
    vec2( -0.26496911, -0.41893023 ),
    vec2( 0.79197514, 0.19090188 ),
    vec2( -0.24188840, 0.99706507 ),
    vec2( -0.81409955, 0.91437590 ),
    vec2( 0.19984126, 0.78641367 ),
    vec2( 0.14383161, -0.14100790 )
);

float random(vec4 seed)
{
    float dot_product = dot(seed, vec4(12.9898,78.233,45.164,94.673));
    return fract(sin(dot_product) * 43758.5453);
}

void main()
{
    vec3 position = vec3(texelFetch(uGPosition, ivec2(gl_FragCoord.xy), 0));
    vec3 normal = vec3(texelFetch(uGNormal, ivec2(gl_FragCoord.xy), 0));

    vec3 ka = vec3(texelFetch(uGAmbient, ivec2(gl_FragCoord.xy), 0));
    vec3 kd = vec3(texelFetch(uGDiffuse, ivec2(gl_FragCoord.xy), 0));
    vec4 ksShininess = texelFetch(uGGlossyShininess, ivec2(gl_FragCoord.xy), 0);
    vec3 ks = ksShininess.rgb;
    float shininess = ksShininess.a;

    vec3 eyeDir = normalize(-position);

    float distToPointLight = length(uPointLightPosition - position);
    vec3 dirToPointLight = (uPointLightPosition - position) / distToPointLight;
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

    vec4 positionInDirLightScreen = uDirLightViewProjMatrix * vec4(position, 1); // Compute fragment position in NDC space of light
    vec3 positionInDirLightNDC = vec3(positionInDirLightScreen / positionInDirLightScreen.w) * 0.5 + 0.5; // Homogeneize + put between 0 and 1
    //float dirLightVisibility = textureProj(uDirLightShadowMap, vec4(positionInDirLightNDC.xy, positionInDirLightNDC.z - uDirLightShadowMapBias, 1.0), 0.0);

    float dirLightVisibility = 0.0;
    float dirSampleCountf = float(uDirLightShadowMapSampleCount);
    int step = max(1, 16 / uDirLightShadowMapSampleCount);
    for (int i = 0; i < uDirLightShadowMapSampleCount; ++i)
    {
        // Noisy shadows:
        // int index = int(dirSampleCountf * random(vec4(gl_FragCoord.xyy, i))) % uDirLightShadowMapSampleCount;

        // Blurred shadows:
        int index = (i + step) % uDirLightShadowMapSampleCount;

        dirLightVisibility += textureProj(uDirLightShadowMap, vec4(positionInDirLightNDC.xy + uDirLightShadowMapSpread * poissonDisk[index], positionInDirLightNDC.z - uDirLightShadowMapBias, 1.0), 0.0);
    }
    dirLightVisibility /= dirSampleCountf;

    int face = 0;
    vec3 pointLightToPos = vec3(uPointLightViewMatrix * vec4(-dirToPointLight, 0));
    vec3 absDirToPointLight = abs(pointLightToPos);
    float maxComp = max(absDirToPointLight.x, max(absDirToPointLight.y, absDirToPointLight.z));

    if (maxComp == absDirToPointLight.x)
    {
        if (pointLightToPos.x < 0)
            face = 1;
    }
    else if (maxComp == absDirToPointLight.y)
    {
        if (pointLightToPos.y >= 0)
            face = 2;
        else
            face = 3;
    }
    else
    {
        if (pointLightToPos.z >= 0)
            face = 4;
        else
            face = 5;
    }

    vec4 positionInPointLightScreen = uPointLightViewProjMatrix[face] * vec4(position, 1);
    vec3 positionInPointLightNDC = vec3(positionInPointLightScreen / positionInPointLightScreen.w) * 0.5 + 0.5;
    float depthBlockerInPointSpace = texture(uPointLightShadowMap, pointLightToPos).r;
    float pointLightVisibility = positionInPointLightNDC.z < depthBlockerInPointSpace + uPointLightShadowMapBias ? 1.0 : 0.0;

    fColor = ka;
    fColor += kd * (dirLightVisibility * uDirectionalLightIntensity * max(0.f, dot(normal, uDirectionalLightDir)) + pointLightVisibility * pointLightIncidentLight * max(0., dot(normal, dirToPointLight)));
    fColor += ks * (dirLightVisibility * uDirectionalLightIntensity * dothDirLight + pointLightVisibility * pointLightIncidentLight * dothPointLight);
}