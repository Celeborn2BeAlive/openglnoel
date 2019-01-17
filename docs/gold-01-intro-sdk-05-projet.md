---
id: gold-01-intro-sdk-05-projet
title: Projet - Un viewer glTF
---

L'objectif de ce projet est de réaliser en C++ un viewer de modèles au format glTF, similaire à celui de [Sketchfab](https://sketchfab.com/).

> Je mettrais prochainement sur cette page des liens vers les modèles 3D sketchfab que j'utiliserais pour tester votre projet. De cette manière vous pourrez utiliser les même pour faire vos tests.

## Contraintes

- En C++
- OpenGL 4+
- Dans une application de votre fork du repository des TDs
- En monome ou binome (pas en trinome, pas en quatrome, pas en quicome, et pas à zero).
- Date de rendu: **31 Mars 2019 minuit au plus tard**
- Modalité de rendu: Vous m'enverrez un mail indiquant le numéro du commit correspondant à votre rendu de projet sur votre repository. Ce commit devra avoir été pushé avant l'heure  limite de rendu.
- Vous joindrez au mail un rapport court (moins de deux pages) en pdf m'indiquant la liste des fonctionnalités implémentées, les difficultés rencontrées, et les eventuels problemes non résolu.

Conseil pour travailler en binome sur Git: vous pouvez vous faire des pull request entre differents fork.

## Fonctionalités de base

Voici la liste minimale des fonctionalités à implementer:

- Chargement d'un modèle glTF dont le chemin est passé sur la ligne de commande (c'est à dire à lire dans argv[1]).
- Gestion au minimum d'une scene statique
- Caméra trackball initialisée en face de l'objet (up = +Y, regarde vers -Z de la scène, et située à une distance du centre de l'objet tel que l'objet soit entierement visible)
- Controle de la caméra similaire à Sketchfab: scroll = zoom/dezoom, left press + drag = rotation autour du centre de la caméra, right press + drag ou middle press + drag = deplacement du centre de la caméra dans son plan X-Y (permet de changer le centre de rotation).
- Une lumière directionnelle modifiable via ImGUI
- Une shadow map avec percentage closest filtering sur la lumière directionnelle
- Gamma correction controlable depuis la GUI (par defaut un gamma à 2.2).
- Deferred rendering, avec possibilité d'afficher les couches du GBuffer depuis la GUI à la place du rendu final
- Materiaux diffus + emission, avec pour couleur diffuse la **baseColor** du materiau (voir plus bas dans la partie glTF) et pour ambiant son **emissionColor**.

Si tous ces points sont implémentés correctement, alors **vous aurez une note de 12/20**. Pour augmenter cette note, il faudra implementer des techniques non vues en TD. Voir une liste de proposition plus bas.

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

**A minima je vous demande de gérer le lobe diffus**, comme vu en TD, avec comme couleur le paramètre baseColor du materiaux (une texture la plupart du temps). En plus de ça l'emission est a ajouter (c'est de l'ambiant en gros). Vous aurez donc quelque chose de la forme:

$$
outColor = emissionColor + baseColor * \cos(\theta_i) * L
$$

ou \\(\theta\\) est l'angle entre la normale et la direction vers la lumière, et \\(L\\) l'intensité lumineuse.

L'implémentation du modele complet de materiaux permettra l'obtention de points supplémentaires (voir plus bas).

Le format glTF permet de representer des modèles animés. De la même manière, l'implementation de la gestion des animations des modeles permettra d'obtenir des points supplémentaires.

## Exemples de techniques avancées pour gratter des points

Comme je l'ai dit plus haut, l'implementation des fonctionnalités de base est noté sur 12. Les 8 points restant peuvent être obtenu via l'implementation de techniques de rendu avancées que l'on a pas vu en TD. Heuresement pour vous le net regorge de tutoriaux (le site [https://learnopengl.com/](https://learnopengl.com/) est une mine d'or). Voici une liste d'idées et les points associés.

<span class="badge warning"></span> Ne pas commencer l'implementation des techniques avancées tant que les fonctionnalités de base ne sont pas traitées.

### Gestion des animations (2 pts)

- Chargement et rendu sur GPU des animations du GLTF
- Possibilité de controler depuis la GUI la timeline d'animation et les differentes animations à jouer

### Modèle de shading PBR (2 pts)

- Implementation du modèle complet metallicRoughness (avec éclairage depuis la directional light)

References / Aide:
- https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md#appendix-b-brdf-implementation
- https://learnopengl.com/PBR/Theory
- https://learnopengl.com/PBR/Lighting

### Lighting avancé depuis une environment light (3 pts, a faire après le modèle de shading PBR)

- Chargement d'une texture d'environment
- Calcul du terme diffus depuis cette env light avec une technique par harmoniques spheriques
- Calcul du terme speculaire avec prefiltrage et échantillonage

References / Aide:
- https://learnopengl.com/PBR/IBL/Diffuse-irradiance
- https://learnopengl.com/PBR/IBL/Specular-IBL

### Chargement et rendu de lumières ponctuelle en respectant l'extension glTF KHR_lights_punctual (1 pt)

Par défaut le format glTF ne permet pas de stocker des lumières. Mais l'extension KHR_lights_punctal definit une syntaxe pour traiter des points lights, spot lights et directional lights. La bibliothèque tinygltf semble les charger dans sa structure de donnée (sinon il faudra coder le chargement).

- Charger les lumières du fichier d'entrée avec tinygltf ou avec votre code si necessaire
- Faire le rendu de ces lumières en plus de la directional light de base (et de l'environment light si vous avez choisit de l'implementer)

Il sera peut être difficile de trouver des scènes contenant des lumières. Dans ce cas ajoutez en a la main dans le fichier json de scènes existantes.

References / Aide:
- https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_lights_punctual
- https://learnopengl.com/Lighting/Light-casters

### Normal mapping (1 pt)

Technique assez simple à implementer et très documentée sur le net.

References / Aide:
- https://learnopengl.com/Advanced-Lighting/Normal-Mapping

### Screen space ambiant occlusion (2 pt)

Même chose que pour le normal mapping, assez simple à coder, sympa visuellement et bien documenté.

References / Aide:
- https://learnopengl.com/Advanced-Lighting/SSAO

### Bloom (1 pt)

Post process sympa mais un peu cheap permettant un halo autour des objets emissifs. L'idée est de rendre une passe d'emission et de la blurrer.

References / Aide:
- https://learnopengl.com/Advanced-Lighting/Bloom

### Rendu de la transparence (3 pt)

Le format glTF permet de definir une opacité sur le materiaux des objets. L'idée est de rendre la transparence des objets. Pour ça il faudra passer par une passe de rendu forward qui ne rend que les objets transparent par dessus le rendu final du deferred (il faudra partager le depth buffer entre le deferred et le forward). Il faudra également trier les objets transparent selon leur profondeur par rapport à la camera.

Si on ne fait pas le rendu des objets transparent avec la technique deferred rendering, c'est parce que le GBuffer ne contient que les données d'un point par pixel. Or la transparence conduit à traiter plusieurs points par pixel, ce qui est possible en forward.

References / Aide:
- https://learnopengl.com/Advanced-OpenGL/Blending

### Shadow mapping depuis les points light (3 pt)

En TD nous avons vu le shadow mapping sur une directional light. Il est également possible de faire cette technique avec une point light. Pour cela il faut rendre la scene 6 fois sur chaque face d'une cube map entourant la point light. Ce n'est pas si compliqué que ça, mais assez facile de foutre le bordel dans les etats OpenGL.

References / Aide:
- https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows

### Frustum culling (1 ou 3 pts)

Le frustum culling permet d'eviter d'essayer de dessiner des objets situé en dehors du champs de vue de la caméra. Il est possible de faire ce culling sur CPU avant de faire les appels de dessin, ou sur GPU et dans ce cas utiliser l'indirect rendering pour gérer les commandes de dessin sur GPU en faisant le culling (cette generation doit se faire dans un compute shader).

- Frustum culling sur CPU: 1pt
- Frustum culling sur GPU: 2pts supplémentaires

References / Aide:

- http://www.lighthouse3d.com/tutorials/view-frustum-culling/
- https://www.khronos.org/opengl/wiki/Vertex_Rendering#Indirect_rendering

### Autres techniques en vrac

Les techniques suivantes sont issues de la recherche et il est plus difficile de trouver de la documentation sur leur implementation (la plupart du temps il n'y a que l'article scientifique associé). Chacune rapportera 5 points.

- Reflective Shadow Maps (a faire après "Shadow mapping depuis les points light" car plus compliqué): Permet le rendu de l'éclairage indirect après un rebond depuis la lumière
    - http://www.klayge.org/material/3_12/GI/rsm.pdf
- Rendu d'area lights: plus physiques, les area lights en temps réel permettent un rendu beaucoup plus réaliste
    - https://eheitzresearch.wordpress.com/415-2/
    - https://blog.selfshadow.com/publications/s2016-advances/
- Clustered Shading: Permet de rendre en temps réel beaucoup de lumières
    - http://www.cse.chalmers.se/~uffe/clustered_shading_preprint.pdf
    - http://www.humus.name/Articles/PracticalClusteredShading.pdf
    - http://efficientshading.com/2014/01/01/efficient-real-time-shading-with-many-lights/
    - http://efficientshading.com/2013/08/01/practical-clustered-deferred-and-forward-shading/

Si vous avez d'autres idées, mettez les en commentaire. Si je trouve ça assez challenge je les ajouterais à la liste avec une quantité de points associée.