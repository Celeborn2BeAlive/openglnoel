---
id: gold-01-intro-sdk-05-projet
title: Projet - Un viewer glTF
---

L'objectif de ce projet est de réaliser en C++ un viewer de modèles au format glTF, similaire à celui de [Sketchfab](https://sketchfab.com/).

Les contraintes:

- En C++
- OpenGL 4+
- Dans une application de votre fork du repository des TDs
- En monome ou binome (pas en trinome, pas en quatrome, pas en quicome, et pas à zero).

Voici la liste minimale des fonctionnalités à implementer:

- Chargement d'un modèle glTF dont le chemin est passé sur la ligne de commande (c'est à dire à lire dans argv[1]).
- Gestion au minimum d'une scene statique
- Caméra trackball initialisée en face de l'objet (up = +Y, regarde vers -Z de la scène, et située à une distance du centre de l'objet tel que l'objet soit entierement visible)
- Controle de la caméra similaire à Sketchfab: scroll = zoom/dezoom, left press + drag = rotation autour du centre de la caméra, right press + drag ou middle press + drag = deplacement du centre de la caméra dans son plan X-Y (permet de changer le centre de rotation).
- Une lumière directionnelle modifiable via ImGUI
- Une shadow map avec percentage closest filtering sur la lumière directionnelle
- Gamma correction controlable depuis la GUI (par defaut un gamma à 2.2).
- Deferred rendering, avec possibilité d'afficher les couches du GBuffer depuis la GUI à la place du rendu final
- Materiaux diffus + emission, avec pour couleur diffuse la **baseColor** du materiau (voir plus bas dans la partie glTF) et pour ambiant son **emissionColor**.

Si tous ces points sont implémentés correctement, alors vous aurez une note de 12/20. Pour augmenter cette note, il faudra implementer des techniques non vues en TD. Voir une liste de proposition plus bas.

## Le format glTF

Ce format développé par Khronos group permet d'échanger facilement des scènes statiques ou animées, avec un modèle de matériaux dit PBR (physically based rendering). La première chose à faire est d'aller lire la spécification sur [le repository github de glTF](https://github.com/KhronosGroup/glTF). Il y a également une overview permettant de rapidement comprendre les concepts du format.

Les modèles disponibles sur Sketchfab sont automatiquement converti en glTF, ce qui vous permettra d'essayer votre viewer sur un grand nombre d'exemple. [Ce lien propose les modèles gratuits](https://sketchfab.com/models?features=downloadable&sort_by=-likeCount) triés par popularité, et [ce lien](https://sketchfab.com/features/gltf) propose des modèles payants et gratuit selectionnés par Sketchfab pour leur qualité.

Pour charger un fichier glTF, vous pouvez utiliser la biliothèque [tinygltf](https://github.com/syoyo/tinygltf), déjà présente dans le template. C'est une bibliotheque header only. Dans un unique cpp il faut mettre les lignes:

```cpp
#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>
```

Ce qui defini l'implementation de la lib. Dans les autres cpp ou hpp qui utilise la lib, il suffit juste d'inclure le header de tinygltf, sans le define.

Le modèle de materiaux utilisé par glTF est dit PBR, car il se base sur un modèle mathématique physiquement plausible. En gros on a un lobe d'emission, un lobe diffus et un lobe speculaire, ce dernier étant controllé par un paramètre metallic et un paramètre de rugosité (roughness). Les equations sont détaillées [sur ce lien](https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md#appendix-b-brdf-implementation). En plus de ça on a un paramètre d'opacité pour rendre plus ou moins opaque l'objet, et une normal map.

A minima je vous demande de gérer le lobe diffus, comme vu en TD, avec comme couleur le paramètre baseColor du materiaux (une texture la plupart du temps). En plus de ça l'emission est a ajouter (c'est de l'ambiant en gros). Vous aurez donc quelque chose de la forme:

$$
outColor = emissionColor + baseColor * \cos(\theta)
$$

ou \\(\theta\\) est l'angle entre la normal et la direction vers la lumière.