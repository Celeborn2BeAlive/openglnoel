---
id: silver-05-post-processing-02-gamma-correction
title: Gamma Correction
---

Un premiere filtre simple que nous allons appliquer à l'image est la **gamma correction**. Ce filtre corrige les couleurs d'une image pour tenir compte du rendu non linéaire des couleurs qu'un moniteur affiche. Pour plus de détails consultez [l'article wikipedia](http://en.wikipedia.org/wiki/Gamma_correction) ou [cet article](http://www.geeks3d.com/20101001/tutorial-gamma-correction-a-story-of-linearity/).

## Gamma correction

Le principe de la correction gamma est d'élever la valeur de tous les pixels (les 3 composantes R, G et B) à la puissance (1 / G) où G est la valeur de gamma. En general on prend G = 2.2, mais cette valeur doit être modifiable en temps réel dans votre application. Ce processus à pour effet de réhausser fortement les valeurs sombres par rapport aux valeurs claires.

Algorithmiquement cela donne:

- Pour chaque pixel (i, j)
    - outImage(i, j) = pow(inputImage(i, j), 1 / G)

Facile, non ? On va faire ça sur GPU.

## Le Compute Shader

Ajoutez un fichier **gammaCorrect.cs.glsl** au repertoire des shaders de l'application a remplissez le avec le code suivant (à réécrire, pas de copier-coller):

```glsl
#version 430

layout(local_size_x = 1, local_size_y = 1) in;

void main() {
    // todo
}
```

Ce code à la structure de base d'un compute shader. Un compute shader est destiné à être lancé en parallèle sur un grand nombre d'unités de calcul. Ces unités de calcul correspondent aux coeurs GPU et sont les même qu'en Cuda.

Les unités sont répartis en groupes. La ligne:

```glsl
layout(local_size_x = 1, local_size_y = 1) in;
```

définit la taille des groupes de calcul, ici 1x1. Il est possible de partager des données entre différentes unités d'un même groupe afin de construire des algorithmes parallèles élaborés. Ici nous faisons simple en spécifiant des groupes de taille 1x1 car nous n'avons pas de donnés à partager pour la gamma correction (les executions sont completement indépendantes).

Les compute shaders possèdent plusieurs variables d'entrée prédéfinies:

```glsl
in uvec3 gl_NumWorkGroups; // Nombre de groupes lancés sur les 3 dimensions
in uvec3 gl_WorkGroupID; // ID du groupe actuel
in uvec3 gl_LocalInvocationID; // ID de l'unité de calcul actuelle au sein de son groupe
in uvec3 gl_GlobalInvocationID; // ID de l'unité de calcul actuelle au sein de toutes les unités
in uint  gl_LocalInvocationIndex; // ID ramené entre 0 et le nombre d'unité de calcul par groupe
```

Pour plus de détails sur les compute shaders, il est important de [lire cette page du wiki OpenGL](https://www.khronos.org/opengl/wiki/Compute_Shader).

La variable qui va nous servir pour faire la gamma correction est **gl_GlobalInvocationID**. L'idée est d'avoir une unité de calcul lancée pour chaque pixel de l'image à traiter. C'est cette variable qui nous permet d'identifier l'unité de calcul actuelle, et donc le pixel à traiter.

Ajoutez la ligne:

```glsl
ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
```

au main du shader. Ici on récupère les coordonnées du pixel à traiter à partir de l'ID de l'unité de calcul courante.

Il va maintenant falloir traiter le pixel. Pour ça il faut lire sa valeur, qui doit venir d'une image. Pour passer des informations à un compute shader, il faut passer par des variables uniformes. Une solution est donc de prendre en uniform un sampler2D qui nous permettra de lire la texture beauty et de lui appliquer le gamma.

En réalité plutot que d'utiliser un sampler2D, nous allons utiliser un nouveau type de variable GLSL permettant de lire/écrire des images: un image2D.

Dans le shader avant le main ajoutez la ligne:

```glsl
layout(rgba32f, binding = 0) uniform readonly image2D uInputImage;
```

Cette déclaration peut paraitre compliquée, on va donc la décomposer:

- rgba32f indique le type de l'image que nous allons fournir en entrée. GLSL impose de spécifier ce type lorsque l'image n'est pas en writeonly.
- binding = 0 indique que l'image coté CPU devra être bindée sur l'image unit 0 (à ne pas confondre avec les textures unit, mais le système est similaire) avant d'executer le shader.
- readonly indique que nous allons seulement lire l'image
- et image2D est simplement le type

Cette variable uniform va nous permettre de lire l'image d'entrée, mais il faut également une image de sortie. Ajoutez à la suite:

```glsl
layout(binding = 1) uniform writeonly image2D uOutputImage;
```

Ici le binding est 1, on devra donc binder l'image de sortie sur l'image unit 1 avant d'executer le shader. On specifie que l'image est en writeonly, on a donc pas besoin de specifier le type de l'image.

A nouveau, [une page du wiki à lire](https://www.khronos.org/opengl/wiki/Image_Load_Store) concernant les images coté GLSL et CPU.

Enfin coté uniformes, ajoutez une variable **float uGammaExponent** qui nous servira à indiquer au shader la valeur de gamma à appliquer.

Il faut maintenant completer le main. Pour cela on doit:

- lire la valeur du pixel dans l'image d'entrée
- elever cette valeur à la puissance uGammaExponent
- écrire la nouvelle valeur dans l'image de sortie.

3 lignes de code donc. Pour cela utilisez les fonctions GLSL suivantes:

```glsl
vec4 imageLoad(image2D image​, ivec2 pixelCoords); // Lit un pixel
void imageStore(image2D image, ivec2 pixelCoords, vec4 data); // Ecrit un pixel
vec4 pow(vec4 valeur, vec4 exponent); // Fonction puissance
```

## Chargement du Compute Shader

Coté C++ il faut charger le shader et récuperer les uniformes.

Commencez par ajouter la variable membre **glmlv::GLProgram m_gammaCorrectionProgram** à la classe application. A l'initialisation utilisez la fonction **glmlv::loadProgram** pour charger le shader **gammaCorrect.cs.glsl** (un seul chemin à mettre dans les accolades donc).

Au niveau des uniformes il n'y a en fait qu'une seule location à récupérer: celle de **uGammaExponent**, à stocker dans une variable membre.

Les deux autres (uInputImage et uOutputImage) n'ont pas à être récupérer car nous avons spécifié leur binding directement dans le shader. Si ca n'avait pas été le cas, il aurait fallut récupérer leur location et utiliser glUniform1i pour spécifier les images unit depuis le code C++ (comme pour les samplers).

A ce stade essayez de lancer l'executable afin de verifier que le shader compile bien.

## La texture de rendu

Nous allons avoir besoin d'une nouvelle texture et d'un FBO associé pour stocker l'image après gamma correction. Comme vous l'avez fait en introduction pour la beauty, rajoutez des variables membre **GLuint m_GammaCorrectedBeautyTexture** et **GLuint m_GammaCorrectedBeautyFBO** à la classe Application. Faites ce qu'il faut à l'initialisation pour que cette nouvelle texture et ce FBO soient fonctionnel.

## L'appel au Compute Shader

Nous avons tout en place pour appeler le compute shader dans le code de rendu, après la shading pass:

1. Utilisez le nouveau programme avec la methode use()
2. Envoyez l'uniforme uGammaExponent avec la fonction glUniform1f. Pour cela definissez une variablle gamma en haut de la fonction (initialisée à 2.2), et envoyez au shader la valeur (1 / gamma). Faites en sorte que gamma soit modifiable depuis la GUI en utilisant la fonction **ImGui::InputFloat**
3. Bindez les deux images sur les image units 0 et 1 en utilisant la fonction OpenGL **glBindImageTexture** (voir ci dessous pour le prototype).
4. Enfin appelez la fonction **glDispatchCompute** pour lancer le compute shader.

La fonction **glBindImageTexture** a le prototype suivant:
```glsl
void glBindImageTexture(
    GLuint unit,
    GLuint texture,
    GLint level,
    GLboolean layered,
    GLint layer,
    GLenum access,
    GLenum format);
```

Il faut appeler cette fonction 2 fois avec unit = 0 et 1 alternativement, et texture = m_BeautyTexture et m_GammaCorrectedTexture alternativement.
Le paramètre access doit être mis à GL_READ_ONLY pour le premier appel et GL_WRITE_ONLY pour le second.

Les autres paramètres doivent prendre les valeurs level = 0, layered = GL_FALSE, layer = 0 et format = GL_RGBA32F pour les deux appels.

## Le blit à l'écran

Il ne reste plus qu'a afficher l'image gamma corrected à l'écran. Pour cela remplacez le binding de m_BeautyFBO par celui de m_GammaCorrectedBeautyFBO avant le blit à l'écran.

Lancez l'executable et vérifiez que la modification du gamma depuis la GUI affecte bien l'apparence de l'image (un gamma plus elevé doit rendre l'image plus claire).