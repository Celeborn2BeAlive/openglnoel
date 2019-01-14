---
id: silver-04-shadow-mapping-02-dir-shadow-map
title: Directional Shadow Map
---

Nous allons commencer par l'utilisation du shadow mapping pour une lumière directionnelle, qui est plus simple.

Il y a globalement quatre partie à coder:

- Initialisation: Allocation des données OpenGL nécessaire à l'algorithme
- Shaders: Coder les shaders nécessaire à la création de la shadow map.
- Création de la shadow map: A faire au rendu des que la direction de la lumière est modifiée. Il s'agit de calculer la depth map selon le point de vue de la light.
- Utilisation de la shadow map: A faire dans la shading pass. Il s'agit avant d'éclairer un fragment vu par la caméra de vérifier s'il est visible depuis la light en utilisant la depth map précalculée.

## Initialisation

Dans la classe Application, ajoutez 4 variables:

```cpp
GLuint m_directionalSMTexture;
GLuint m_directionalSMFBO;
GLuint m_directionalSMSampler;
int32_t m_nDirectionalSMResolution = 512;
```

La première est destinée à stocker un identifiant de texture OpenGL qui contiendra la depth map selon le point de vue de la light.

La deuxième est destinée à stocker un identifiant de framebuffer OpenGL qui nous permettra de dessiner la depth map.

La troisème est destinée à stocker un identifiant de sampler OpenGL qui nous permettra de lire la depth map depuis un shader.

Enfin la dernière stocke simplement la résolution de la depth map (ici 512x512).

Dans le constructeur de l'application, créez à la texture m_directionalSMTexture, de target GL_TEXTURE_2D et de format interne GL_DEPTH_COMPONENT32F.

Créez le framebuffer m_directionalSMFBO et attachez lui la texture m_directionalSMTexture sur l'attachment GL_DEPTH_ATTACHMENT (voir le code de l'initialisation du GBuffer du deferred shading pour exemple).

Vérifiez la validité du framebuffer, puis débindez le framebuffer.

Créez enfin le sampler m_directionalSMSampler avec le code suivant:

```cpp
glGenSamplers(1, &m_directionalSMSampler);
glSamplerParameteri(m_directionalSMSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glSamplerParameteri(m_directionalSMSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glSamplerParameteri(m_directionalSMSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
glSamplerParameteri(m_directionalSMSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
```

Fonctions GL à utiliser:

| Sans DSA                             | DSA |
| ------------------------------------ | ----------- |
| glGenTextures | glCreateTextures |
| glBindTexture |  |
| glTexStorage2D | glTextureStorage2D |
| glGenFramebuffers                         | glCreateFramebuffers |
| glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo) | |
| glFramebufferTexture2D                    | glNamedFramebufferTexture |
| glDrawBuffers | glNamedFramebufferDrawBuffers |
| glCheckFramebufferStatus | glCheckNamedFramebufferStatus |
| glGenSamplers | glCreateSamplers |
| glSamplerParameteri | glSamplerParameteri |

## Shaders

Ajoutez deux nouveaux shaders *directionalSM.vs.glsl* et *directionalSM.fs.glsl*.

L'objectif du vertex shader est de transformer le sommet d'entrée dans l'espace de la light. Pour cela, on suppose que l'application fournie une matrice ViewProjection adaptée et le code est simplement le suivant:

```glsl
// directionalSM.vs.glsl
#version 330

layout(location = 0) in vec3 aPosition;
uniform mat4 uDirLightViewProjMatrix;

void main()
{
    gl_Position =  uDirLightViewProjMatrix * vec4(aPosition, 1);
}
```

Le fragment shader n'a rien a faire car tout ce qui nous interesse c'est la depth des fragments. Or la depth est directement écrite par la carte graphique dans le depth buffer après l'execution du fragment shader. On se contente donc d'écrire un fragment shader minimal qui ne sort que du noir:

```glsl
// directionalSM.fs.glsl
#version 330
out vec3 fColor;
void main()
{
    fColor = vec3(0);
}
```

Dans l'application, ajoutez un nouveau programme et une variable pour stocker la location de l'uniform *uDirLightViewProjMatrix*:

```cpp
glmlv::GLProgram m_directionalSMProgram;
GLint m_uDirLightViewProjMatrix;
```

Dans le constructor, compilez le programme à partir des deux nouveaux shaders et récupérez à la location avec *glGetUniformLocation*.

## Création de la shadow map

Il faut à présent calculer la shadow map dans la boucle de rendu. Attention: ce calcul necessite un rendu de la scène, qui peut être coûteux. Il faut donc faire attention à ne recalculer la shadow map que si la light change de direction, par exemple en utilisant un booléen. La structure générale de la méthode run() devient donc:

```cpp
int Application::run()
{
    [...]
    bool directionalSMDirty = true;

    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        [...]
        if (directionalSMDirty)
        {
            // Calcul de la shadow map (**)
            [...]

            directionalSMDirty = false; // Pas de calcul au prochain tour
        }

        // Rendu
        [...]

        // Pseudo code dans le dessin de la GUI:
        if (directional_light_change)
        {
            directionalSMDirty = true; // Il faut recalculer la shadow map
        }
        [...]
    }
}
```

Mettez en place cette structure de code par dessus le code actuel de rendu du deferred.

A la suite du commentaire *// Calcul de la shadow map (**)*, ajoutez le code suivant:

```cpp
m_directionalSMProgram.use();

glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_directionalSMFBO);
glViewport(0, 0, m_nDirectionalSMResolution, m_nDirectionalSMResolution);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

glUniformMatrix4fv(m_uDirLightViewProjMatrix, 1, GL_FALSE, glm::value_ptr(dirLightProjMatrix * dirLightViewMatrix));

glBindVertexArray(m_SceneVAO);

// We draw each shape by specifying how much indices it carries, and with an offset in the global index buffer
for (const auto shape : m_shapes) {
    glDrawElements(GL_TRIANGLES, shape.indexCount, GL_UNSIGNED_INT, (const GLvoid*)(shape.indexOffset * sizeof(GLuint)));
}

glBindVertexArray(0);

glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
```

Ce code met en place le programme pour dessiner la shadow map, bind le FBO sur lequel la shadow map est attachée, puis dessine la scène. Logiquement, après ce dessin, la texture m_directionalSMTexture (attachée à m_directionalSMFBO) doit contenir la shadow map.

Dans ce code, il vous manque le calcul des deux matrices dirLightProjMatrix et dirLightViewMatrix, qui est le suivant (à faire au début de la boucle de rendu car ces variables serviront aussi à la shading pass):

```cpp
static const auto computeDirectionVectorUp = [](float phiRadians, float thetaRadians)
{
    const auto cosPhi = glm::cos(phiRadians);
    const auto sinPhi = glm::sin(phiRadians);
    const auto cosTheta = glm::cos(thetaRadians);
    return -glm::normalize(glm::vec3(sinPhi * cosTheta, -glm::sin(thetaRadians), cosPhi * cosTheta));
};

const auto sceneCenter = 0.5f * (m_BBoxMin + m_BBoxMax);
const float sceneRadius = m_SceneSizeLength * 0.5f;

const auto dirLightUpVector = computeDirectionVectorUp(glm::radians(m_DirLightPhiAngleDegrees), glm::radians(m_DirLightThetaAngleDegrees));
const auto dirLightViewMatrix = glm::lookAt(sceneCenter + m_DirLightDirection * sceneRadius, sceneCenter, dirLightUpVector); // Will not work if m_DirLightDirection is colinear to lightUpVector
const auto dirLightProjMatrix = glm::ortho(-sceneRadius, sceneRadius, -sceneRadius, sceneRadius, 0.01f * sceneRadius, 2.f * sceneRadius);
```

La matrice de projection du lumière directionelle est orthographique car une telle lumière voit "en parallèle" (tous les points sont illuminés selon la même direction). On fait en sorte que la light voit l'ensemble de la scène en la plaçant au bord de la bounding sphere de la scène (d'ou le calcul du centre et du rayon de la scène). Il vous sera peut être necessaire de stocker m_BBoxMin et m_BBoxMax si ce n'est pas déjà fait, qui peuvent être obtenu au moment du chargement de la scène.

## Utilisation de la shadow map

Une fois la shadow map calculée, la shading pass peut l'utiliser pour calculer la visibilité d'un fragment pour la light concernée.

Il faut tout d'abord modifier le fragment shader de la shading pass (shadingPass.fs.glsl) afin d'utiliser la shadow map.
Ajoutez les uniform suivantes:

```glsl
uniform mat4 uDirLightViewProjMatrix;
uniform sampler2D uDirLightShadowMap;
uniform float uDirLightShadowMapBias;
```

La matrice permet de passer les fragment vus par la caméra dans l'espace projeté de la light. Une fois cette projection faite, on peut lire dans la shadow map pour obtenir la depth enregistrée dans le pixel concerné. Cette depth doit alors être comparée à la depth du fragment courant, toujours selon le point de vue de la light, pour savoir s'il est visible depuis la light.

Le code est le suivant:

```glsl
vec4 positionInDirLightScreen = uDirLightViewProjMatrix * vec4(position, 1);
vec3 positionInDirLightNDC = vec3(positionInDirLightScreen / positionInDirLightScreen.w) * 0.5 + 0.5;
float depthBlockerInDirSpace = texture(uDirLightShadowMap, positionInDirLightNDC.xy).r;
float dirLightVisibility = positionInDirLightNDC.z < depthBlockerInDirSpace + uDirLightShadowMapBias ? 1.0 : 0.0;
```

La première ligne applique la matrice pour projeter la position du fragment courant.

La deuxième light homogénise cette position projetée en divisant par la coordonnée "w". A l'issue de cette opération, toutes les coordonnées sont entre -1 et 1. Afin de pouvoir lire dans la shadow map, on les ramène entre 0 et 1 grace au *0.5 + 0.5. Les coordonnées .xy identifie le pixel à lire dans la shadow map, et la coordonnée .z stocke la depth du fragment selon le point de vue de la light.

La troisème ligne lit la depth stockée dans la shadow map à la position du fragment.

Enfin la dernière ligne compare la depth stockée à la depth du fragment afin de savoir si la light est visible.

On rajoute un biais uDirLightShadowMapBias afin d'éviter les erreurs d'imprecision numériques qui apparaissent sous la forme de "shadow acnée" (mettez ce bias a 0 initialement pour voir le problème).

La variable dirLightVisibility vaut 0 si le fragment est occulté depuis la ligne, 1 sinon. Multipliez cette variable à la contribution de la lumière directionnelle afin de l'ajouter dans fColor.

Une fois les modifications effectuées sur le shader, modifier le code de l'application pour récupérer la location des nouvelles uniformes. Dans la boucle de rendu, settez ces uniforms correctement et bindez la texture de shadow map m_directionalSMTexture ainsi que le sampler m_directionalSMSampler sur une nouvelle texture unit (non utilisée par le GBuffer). Pour le uDirLightShadowMapBias, faites en sorte qu'il soit controlable depuis la GUI.

Enfin il y a un piège pour la matrice uDirLightViewProjMatrix. Le code est un peu différent de la passe précédente puisqu'il ne suffit de pas multiplier dirLightProjMatrix à dirLightViewMatrix pour obtenir cette matrice.

Cela est du au fait que les fragments de la shading pass ne sont pas en espace World mais en espace View de la caméra. Il faut donc qu'ils soient repassés en World avant d'être transformés dans l'espace de la light. Pour cela, il suffit de multiplier à gauche par l'inverse de la View matrix de la caméra:

```cpp
const auto rcpViewMatrix = m_viewController.getRcpViewMatrix(); // Inverse de la view matrix de la caméra
glUniformMatrix4fv(m_uDirLightViewProjMatrix_shadingPass, 1, GL_FALSE, glm::value_ptr(dirLightProjMatrix * dirLightViewMatrix * rcpViewMatrix));
```

## Si ça ne fonctionne pas

A ce stade, votre shadow mapping devrait fonctionner et vous devriez donc voir les ombres portées par les objets de la scène par rapport à la lumière directionnelle.

Néammoins, il peut exister une multitude de raisons pour lesquelles un shadow mapping ne fonctionnerait pas immédiatement (c'est d'ailleur rare qu'il fonctionne du premier coup).

Afin de débogguer, une première approche peut être d'afficher la depth map vue par la caméra. Malheureusement, la depth map ne peut pas être blittée à l'ecran comme les autres textures du GBuffer.

Il faut donc coder un fragment shader dediée à l'affichage de la depth map:

```glsl
// displayDepth.fs.glsl
#version 330

uniform sampler2D uGDepth;

out vec3 fColor;

void main()
{
    float depth = texelFetch(uGDepth, ivec2(gl_FragCoord.xy), 0).r;
    fColor = vec3(depth); // Since the depth is between 0 and 1, pow it to darkness its value
}
```

Ce fragment shader peut être combiné à *shadingPass.vs.glsl* pour former un programme GLSL. En bindant correctment la depth map à afficher, et en dessinant un triangle sur tout l'ecran, on peut ainsi afficher n'importance quel depth map (en particulier celle représentant la shadow map de la light).

Voir sur la branche cheat pour un exemple.