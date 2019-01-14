---
id: gold-03-deferred-renderer-03-shading-pass
title: Shading Pass
---

Il faut maintenant coder la shading pass, qui a pour role d'illuminer chaque pixel en utilisant les textures du GBuffer qui viennents se substituer aux variables d'entrées du fragment shader d'un forward renderer.

## Shaders

Créez deux nouveaux shaders *shadingPass.vs.glsl* et *shadingPass.fs.glsl*.

Le VS est destiner à traiter un simple quad en 2D couvrant tout l'écran, tout ce passera dans le FS.
Voici donc le code du VS:

```glsl
#version 330

layout(location = 0) in vec2 aPosition;

void main()
{
    gl_Position =  vec4(aPosition, 0, 1);
}
```

Pour le FS c'est un peu plus compliqué, mais vous pouvez repartir de celui du forward renderer.
Il faut simplement remplacer les variables *in* et les lectures dans les textures de matériaux par des lectures dans les textures du GBuffer.

On ajoute donc les uniforms suivantes:

```glsl
uniform sampler2D uGPosition;
uniform sampler2D uGNormal;
uniform sampler2D uGAmbient;
uniform sampler2D uGDiffuse;
uniform sampler2D uGlossyShininess;
```

qu'il faut utiliser dans le main du shader pour récupérer les infos géométrique du fragment et les infos de matériaux. 
Pour lire dans les textures du GBuffer, il faut utiliser la fonction GLSL *texelFetch* ainsi que la variable built-in *gl_FragCoord* contenant les coordonnées du pixel courant. Exemple:

```glsl
vec3 position = vec3(texelFetch(uGPosition, ivec2(gl_FragCoord.xy), 0)); // Correspond a vViewSpacePosition dans le forward renderer
```

Ensuite l'algorithme pour calculer la couleur du fragment à partir des lights reste le meme.

Dans le code de l'application, chargez et compilez vos deux shaders dans un second programme GLSL afin de vérifier que vous n'avez pas fait d'erreur de syntaxe. On utilisera ce nouveau programme par la suite dans la boucle de rendu.

A l'initialisation, récupérez les locations des nouvelles uniformes (ainsi que des anciennes, mettez à jour le programme concerné puisqu'on en a deux maintenant).

## Un Quad (ou un triangle, voir la note)

Afin de "lancer" la Shading Pass et pouvoir passer dans le fragment shader pour calculer la couleur de nos pixels, il faut un truc à dessiner à l'écran. Puisque toute la géometrie visible depuis la caméra est déjà encodée dans le GBuffer, il suffira juste de dessiner un quad couvrant tout l'écran pour pouvoir parcourir tous les pixels du GBuffer et faire notre traitement.

Dans la classe Application, ajoutez un nouveau VBO, IBO et VAO. Faite en sorte que ces trois objets stockent ce qu'il faut pour dessiner un quad en 2D sur tout l'ecran (de -1 à 1 sur chacun des axes). Il n'y a que l'attribut de position à envoyer.

> Plutot que de dessiner un quad, qui comporte deux triangle, il est également possible de dessiner un triangle couvrant plus que tout les écrans. L'ensemble des pixels du triangle en dehors de l'écran seront discardés avant même d'arriver dans le fragment shader.
Les coordonnées d'un tel triangle peuvent etres les suivantes: (-1, -1), (3, -1), (-1, 3).
Dessinez le sur papier pour vous en convaincre.
Dans le cas d'un triangle, pas besoin d'IBO, on peut directement appeler *glDrawArrays*.

| Sans DSA                             | DSA |
| ------------------------------------ | ----------- |
| glGenBuffers                         | glCreateBuffers |
| glGenVertexArrays                    | glCreateVertexArrays |
| glBindBuffer(GL_ARRAY_BUFFER, vboID) |
| glBufferStorage                      | glNamedBufferStorage |
|                                      | glVertexArrayVertexBuffer |
|                                      | glVertexArrayAttribBinding |
| glEnableVertexAttribArray            | glEnableVertexArrayAttrib |
| glVertexAttribPointer                | glVertexArrayAttribFormat |
| glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID) | glVertexArrayElementBuffer |

## Au Rendu

Dans la boucle de rendu, remplacez le blit des textures du GBuffer de l'exercice précédent par les étapes suivantes:

- *.use()* sur le programme de la shading pass
- set des uniforms de light (point light et directional light, qui sont normalement dans le fragment shader de la shading pass)
- binding des textures du GBuffer sur différentes texture units (de 0 à 4 inclut)
- set des uniforms correspondant aux textures du GBuffer (chacune avec l'indice de la texture unit sur laquelle la texture correspondante est bindée)
- dessin du quad/triangle

Fonctions OpenGL à utiliser au rendu:

- glUniform3fv, glUniform1i
- glActiveTexture
- glBindTexture
- glBindVertexArray
- glDrawElements (ou glDrawArrays si triangle)
- glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

Après ça vous devriez retrouver le rendu que vous aviez avec le forward renderer.

Vous pouvez essayer de charger de grosse scènes (ou génération aléatoire de plein de géométrie) pour comparer les performances des deux méthodes de rendu.


