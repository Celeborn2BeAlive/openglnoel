---
id: gold-03-deferred-renderer-02-geometry-pass
title: Geometrie Pass
---

Comme indiqué en introduction, la première passe de rendu est la Geometry Pass, dont l'objectif est de "dessiner" dans un GBuffer les informations concernant les objets visible à l'écran.

Pour cela, il va falloir créer les textures OpenGL destinées à contenir ces informations, et les attacher à un **framebuffer object**, qui va permettre d'écrire dans ces textures plutot qu'a l'écran.

Pour ces TPs, dupliquez l'app de l'exercice du foward renderer afin d'avoir une scene chargée et stockée sur GPU pour être rendue.
Nommez la nouvelle app "deferred-renderer".

## Shaders

Nous allons commencer par les shaders car c'est le plus simple.
Renommez les shaders *forward.vs.glsl* et *forward.fs.glsl* en *geometryPass.vs.glsl* et *geometryPass.fs.glsl*.

Il faut ensuite modifier le fragment shader (plus exactement le simplifier).

Tout d'abors modifier les sorties. On avait:

```glsl
out vec3 fColor;
```

a remplacer par:

```glsl
layout(location = 0) out vec3 fPosition;
layout(location = 1) out vec3 fNormal;
layout(location = 2) out vec3 fAmbient;
layout(location = 3) out vec3 fDiffuse;
layout(location = 4) out vec4 fGlossyShininess;
```

On va donc écrire dans 5 textures en tout.

Dans le main du shader, remplacez tout le code d'illumination par des écritures dans les variables de sortie. Ne pas oublier de normaliser *vViewSpaceNormal* avant de l'écrire dans *fNormal*.

A noter que l'on écrit pas les texCoords en sortie car on ne les utilise que pour lire les coefficients ambiant, diffus et glossy des textures de l'objet en cours de rendu. Les texCoords ne sont donc pas necessaire à la Shading Pass puisqu'on écrit directement ces coefficients dans des textures du GBuffer.

Les variables de sortie de matériaux doivent stocker la multiplication du coefficient associé avec la valeur lue dans la texture. Par exemple:

```glsl
uniform vec3 uKd;
uniform sampler2D uKdSampler;

[...]

// Dans le main:
vec3 kd = uKd * vec3(texture(uKdSampler, vTexCoords));
fDiffuse = kd;
```

Enfin, il faut packer la shininess dans le canal alpha de la variable de sortie *fGlossyShininess*.

Dans le code de Application, chargez ces shaders pour tester leur compilation.

## Textures du GBuffer

Dans la classe Application, déclarez un tableau de *GLuint* pour stocker les texture objects, ainsi que l'enum suivant:

```cpp
enum GBufferTextureType
{
    GPosition = 0,
    GNormal,
    GAmbient,
    GDiffuse,
    GGlossyShininess,
    GDepth, // On doit créer une texture de depth mais on écrit pas directement dedans dans le FS. OpenGL le fait pour nous (et l'utilise).
    GBufferTextureCount
};
GLuint m_GBufferTextures[GBufferTextureCount];
```

Dans le constructeur, créer et initialiser ces textures avec comme dimension la taille de la fenêtre.

> Vous pouvez créer toutes les textures en un seul appel à glGenTextures (ou glCreateTextures en DSA) car nos identifiants sont stockés dans un tableau. Il suffit de passer à la fonction le nombre de textures à créer.

Le format à passer à *glTexStorage2D* dépend du type de texture, vous pouvez utiliser le tableau suivant, en correspondance avec l'enum:

```cpp
const GLenum m_GBufferTextureFormat[GBufferTextureCount] = { GL_RGB32F, GL_RGB32F, GL_RGB32F, GL_RGB32F, GL_RGBA32F, GL_DEPTH_COMPONENT32F };
```

Il est inutile de remplir les textures avec *glTexSubImage2D* car elles sont destinées à être remplies par le fragment shader.

Fonctions GL à utiliser:

| Sans DSA                             | DSA |
| ------------------------------------ | ----------- |
| glGenTextures                         | glCreateTextures |
| glBindTexture(GL_TEXTURE_2D, texID) | |
| glTexStorage2D                    | glTextureStorage2D |

## Framebuffer object

Déclarez une variable *GLuint m_FBO* dans la classe Application.

Dans le constructeur, après création des texture objects, créez le FBO (*glGenFramebuffers*) et bindez le sur la cible *GL_DRAW_FRAMEBUFFER*.

Il faut ensuite attacher toutes nos textures au FBO en utilisant la fonction *glFramebufferTexture2D*.

```cpp
glFramebufferTexture2D( GLenum target,
    GLenum attachment,
    GLenum textarget,
    GLuint texture,
    GLint level);
```

La target est la cible sur laquelle est bindée de FBO (*GL_DRAW_FRAMEBUFFER*).

L'attachment est un point d'attache de la forme *GL_COLOR_ATTACHMENT0 + i*, ou *i* est l'index de la texture dans son tableau (de *GPosition* à *GGlossyShininess*).
Pour la texture de profondeur (*GDepth*), elle doit être attachée sur le point *GL_DEPTH_ATTACHMENT*.

Dans notre cas, *textarget* doit être mis à *GL_TEXTURE_2D* car toutes nos textures sont 2D (on pourrait aussi faire le rendu dans des layers de textures 3D, il faudrait alors changer ce paramètre).

Enfin, *level* doit être mis à 0 (c'est le niveau de mipmap dans lequel dessiner).

Une fois les textures attachées, il faut indiquer à OpenGL une association "sortie du fragment shader" vers texture.
Comme vous le verrez au prochain exercice, le fragment shader peut avoir plusieurs sorties indicées avec des *layout(location = i)* (de la meme manière que les entrée du vertex shader).
Il faut dire à OpenGL comment les locations doivent être connectées aux textures du FBO.

Cela passe par la fonction *glDrawBuffers*, qui prend le nombre de sorties du fragment shader et un tableau de *GL_COLOR_ATTACHMENTi* afin de faire la liaison. Nous allons faire assez simple:

```cpp
GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
glDrawBuffers(5, drawBuffers);
```

Ici on va envoyer la sortie *i* du fragment shader vers l'attachment *GL_COLOR_ATTACHMENTi* du FBO.
C'est ce qui parait le plus logique mais, si on voulait, on pourrait tout à fait envoyer la sortie 3 du fragment shader vers la texture attachée sur GL_COLOR_ATTACHMENT0, par exemple.

Utilisez ensuite la fonction *glCheckFramebufferStatus* afin de vérifier si le framebuffer créé est correct (si ce n'est pas le cas, bon debug !).

Enfin débindez le framebuffer de la cible *GL_DRAW_FRAMEBUFFER*.

Fonctions GL à utiliser:

| Sans DSA                             | DSA |
| ------------------------------------ | ----------- |
| glGenFramebuffers                         | glCreateFramebuffers |
| glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo) | |
| glFramebufferTexture2D                    | glNamedFramebufferTexture |
| glDrawBuffers | glNamedFramebufferDrawBuffers |
| glCheckFramebufferStatus | glCheckNamedFramebufferStatus |

## Boucle de rendu

Au rendu assez peu de chose à changer.

Avant de dessiner, binder le framebuffer sur la cible *GL_DRAW_FRAMEBUFFER*. Cela va indiquer à OpenGL que le fragment shader va écrire dans les textures attachées au FBO.

Il faut également faire un *.use()* sur le programme correspondant au aux *geometryPass.glsl*

Après le dessin de la scène, débindez le FBO. Voila.

Pour tester que tout fonctionne bien, on peut blitter les textures du GBuffer à l'écran.

Après le débind du FBO, rebindez le, cette fois ci sur la cible *GL_READ_FRAMEBUFFER*.

Utilisez ensuite la fonction *glReadBuffer*, qui prend en paramètre un *GL_COLOR_ATTACHMENT0 + i*, correspondant à la texture du GBuffer que vous voulez afficher. Vous pouvez par example afficher la texture de normals en passant *GL_COLOR_ATTACHMENT0 + GNormal*. Faites ensuite en sorte de pouvoir choisir la texture à afficher via la GUI (avec des boutons radio, *ImgGui::RadioButton*).

Puis utilisez la fonction *glBlitFramebuffer* qui permet de "copier-coller" (avec filtre) une portion du FBO bindé sur *GL_READ_FRAMEBUFFER* vers le FBO bindé sur *GL_DRAW_FRAMEBUFFER* (c'est à dire l'écran, lorsque rien n'est bindé dessus).

Finalement, débindez le FBO de *GL_READ_FRAMEBUFFER*. A noter qu'en DSA il n'y a rien à binder pour l'étape du blit (il faut quand même binder sur *GL_DRAW_FRAMEBUFFER* avant de dessiner la scène).

Fonctions GL à utiliser:

| Sans DSA                             | DSA |
| ------------------------------------ | ----------- |
| glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo) | glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo) |
| glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo)  |  |
| glReadBuffer | glNamedFramebufferReadBuffer |
| glBlitFramebuffer | glBlitNamedFramebuffer |
