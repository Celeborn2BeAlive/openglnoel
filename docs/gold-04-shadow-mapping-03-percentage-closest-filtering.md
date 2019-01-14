---
id: silver-04-shadow-mapping-03-percentage-closest-filtering
title: Percentage Closest Filtering
---

## Percentage Closest Filtering

### Basique

L'aspect visuel du shadow mapping dépend grandement de la résolution choisie. Pour une résolution de 512, vous avez du constater que le rendu des ombres est très aliasé. Il faut monter à 4096 au moins pour ne plus voir l'aliasing à une distance correcte, qui reste très visible en se rapprochant.

Une manière de palier à cet aliasing est d'utiliser une méthode de filtrage appelée *Percentage Closest Filtering*, qui consiste à moyenner la visibilité dans un voisinage du point afin de flouter/bruiter les ombres sur leur bord.

Cette technique est en partie supportée par le GPU en passant par un sampler de type *sampler2DShadow* en GLSL.

Dans *shadingPass.fs.glsl*, remplacez la déclaration des uniformes du shadow mapping par:

```glsl
uniform mat4 uDirLightViewProjMatrix;
uniform sampler2DShadow uDirLightShadowMap;
uniform float uDirLightShadowMapBias;
```

Puis remplacez le code de calcul de la visibilité par:

```glsl
vec4 positionInDirLightScreen = uDirLightViewProjMatrix * vec4(position, 1); // Compute fragment position in NDC space of light
vec3 positionInDirLightNDC = vec3(positionInDirLightScreen / positionInDirLightScreen.w) * 0.5 + 0.5; // Homogeneize + put between 0 and 1
float dirLightVisibility = textureProj(uDirLightShadowMap, vec4(positionInDirLightNDC.xy, positionInDirLightNDC.z - uDirLightShadowMapBias, 1.0), 0.0);
```

On utilise donc un *sampler2DShadow* à la place d'un *sampler2D* et la fonction *textureProj* à la place de *texture*. Ce code permet d'obtenir un filtrage 2x2 (bilinéaire) sur nos ombres.

Afin que ce shader fonctionne, il faut rajouter deux lignes à l'initialisation du sampler m_directionalSMSampler:

```cpp
glGenSamplers(1, &m_directionalSMSampler);
glSamplerParameteri(m_directionalSMSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glSamplerParameteri(m_directionalSMSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glSamplerParameteri(m_directionalSMSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
glSamplerParameteri(m_directionalSMSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
glSamplerParameteri(m_directionalSMSampler, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE); // Cette ligne
glSamplerParameteri(m_directionalSMSampler, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL) // Et celle ci;
```

Ces lignes définissent le comportement de la fonction *textureProj* dans le shader.

Une fois ces modifications effectuées, lancez votre application et constatez que les ombres sont un peu floutées.

### Arbitraire

Il est possible d'allouer plus loin dans le filtrage des ombres en appliquant un filtre arbitraire lors de la lecture de la shadow map. Cela est plus couteux mais également plus agréable visuellement.

Dans *shadingPass.fs.glsl*, ajoutez les uniform:

```glsl
uniform int uDirLightShadowMapSampleCount;
uniform float uDirLightShadowMapSpread;
```

et le code utilitaires (avant le main):

```glsl
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
```

Le tableau poissonDisk definit 16 échantillons d'une distribution de poisson 2D centrée en (0,0). La fonction random permet d'obtenir un nombre pseudo-aléatoire à partir d'une graine.

Dans le main du shader, remplacez le calcul de la visibilité par:

```glsl
float dirLightVisibility = 0.0;
float dirSampleCountf = float(uDirLightShadowMapSampleCount);
int step = max(1, 16 / uDirLightShadowMapSampleCount);
for (int i = 0; i < uDirLightShadowMapSampleCount; ++i)
{
    // Noisy shadows:
    int index = int(dirSampleCountf * random(vec4(gl_FragCoord.xyy, i))) % uDirLightShadowMapSampleCount;

    dirLightVisibility += textureProj(uDirLightShadowMap, vec4(positionInDirLightNDC.xy + uDirLightShadowMapSpread * poissonDisk[index], positionInDirLightNDC.z - uDirLightShadowMapBias, 1.0), 0.0);
}
dirLightVisibility /= dirSampleCountf;
```

Dans le code de l'application, faites en sorte de pouvoir setter les deux nouvelles uniforms uDirLightShadowMapSampleCount et uDirLightShadowMapSpread via la GUI. La variable uDirLightShadowMapSpread doit être relativement petite (de l'ordre de 0.0005) afin de ne pas trop disperser les ombres.

Essayez ce code. Il devrait vous donner des ombres bruitées mais douces.

Une alternative pour obtenir des ombres floutées plutot que bruitées est de calculer l'index dans la boucle avec:

```glsl
// Blurred shadows:
int index = (i + step) % uDirLightShadowMapSampleCount;
```