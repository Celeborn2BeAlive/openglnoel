---
id: gold-02-forward-renderer-08-plus-loin
title: Geometrie
---

Voici plusieurs choses améliorable pour rendre notre forward renderer plus interessant:

## Normal Mapping

Ajouter une texture de normales dans le fragment shader et faire le necessaire pour charger la texture de normales de chaque objet.

L'utiliser à la place de *uViewSpaceNormal* pour calculer l'illumination.

Vous pouvez vous réferer à [ce tutorial](http://ogldev.atspace.co.uk/www/tutorial26/tutorial26.html).

## Plusieurs Lights

Actuellement le shader ne gère que deux lumières, c'est un peu triste.

Utilisez les [Shader Storage Buffer Objects](https://www.khronos.org/opengl/wiki/Shader_Storage_Buffer_Object) pour accéder à des tableau de directions, positions et intensités depuis le fragment shader, correspondant à un nombre arbitraire de directional et point lights (stockez leur nombre dans des uniforms).

Bouclez sur ces lights et accumulez la contribution de chacune avant de l'écrire dans la variable de sortie *fFragColor*.