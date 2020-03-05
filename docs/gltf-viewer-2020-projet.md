---
id: gltf-viewer-2020-projet
title: Projet
---

> Cette partie est en français car elle concerne les étudiants de l'école d'ingénieur Imac et les Master 2 Images de l'université Paris Est.

- Date de rendu: Mercredi 29 Avril 23h59

## Sujet

Le sujet de ce projet est assez simple: il consiste à améliorer en autonomie votre gltf viewer. Pour cela, voici une liste de possibilités (détaillées plus bas avec des liens vers des tutoriaux en ligne) avec la difficulté associée (en nombre d'étoiles sur 3):

- Normal mapping *
- Many lights *
- Shadow Mapping **
- Deferred Rendering et Post Process (SSAO, DOF) ***
- PBR Lighting ***

Il faut  choisir un de ces sujets et et l'implémenter entierement et correctement:

- Sur le fork de votre repo
- Code propre, structuré et factorisé, mais pas d'over enginerring
- Si la méthode implémentée à des paramètres, ils doivent être controlable via la GUI
- Si la méthode implémentée utilise des données intermédiaires sous forme de textures (shadow mapping, deferred rendering), elles doivent être visualisable via une option dans la GUI.

Un projet à difficulté plus élevée n'aura pas forcément une meilleure note mais sera notée de manière plus indulgente.

Vous pouvez choisir plusieurs sujets mais il ne faut pas s'éparpiller: d'abord en finir un, puis passer à un deuxième s'il vous reste du temps et l'envie.

Si aucun de ces sujets ne vous plait, vous pouvez m'envoyer un mail pour m'en proposer un autre (avant le 15 mars 2020).

## Modalités de rendu 

- Date de rendu: Mercredi 29 Avril 23h59
- Mail
- Rapport d'une page en pdf

Avant la deadline, vous devrez m'envoyer un mail dont le sujet aura pour forme: `OpenGL Projets 2020 - nom prénom`.
Si vous êtes en binome vous mettrez `nom1 prénom1 nom2 prénom2`.

Vous m'indiquerez dans le corps du mail 
- l'URL de votre repo (un des deux si vous etes en binome)
- Le numéro de commit sur lequel votre projet est dans sa version finale.

De mon coté je clonerais votre repo et je me mettrais sur ce commit pour vous noter.

Vous fournirez également un court rapport (1 page en pdf) indiquant:
- Le ou les sujets choisis
- Comment utiliser la fonctionnalité dans votre viewer
- Les difficultés rencontrées
- Les connaissances acquises en implémentant les features

## Détails et resources en ligne

### Normal mapping *

Le normal mapping consiste à utiliser une texture de normale pour rajouter artificiellement des détails aux objets. Le format glTF spécifie une normal map dans son materiaux, il faudra donc bien lire la documentation avant de se lancer dans le code.

Voici un bon tutorial en ligne pour implémenter cette fonctionnalité: https://learnopengl.com/Advanced-Lighting/Normal-Mapping

Le tuto détaille la méthode et donne également d'autres liens.

### Many lights *

Ce sujet est composé de deux parties:
- la gestion de tous les types de light glTF (point, directional, spot)
- la gestion d'un nombre arbitraire de light

Ce tuto détaille chaque type de light: https://learnopengl.com/Lighting/Light-casters

Ce tuto détaille une méthode pour en gérer plusieurs: https://learnopengl.com/Lighting/Multiple-lights

Néammoins, la méthode décrite ne permet pas un nombre arbitraire (les tableaux de light ont une taille max). Si vous voulez aller plus loin, vous pourrez utiliser un **Shader Storage Buffer Object** pour stocker vos lights (à chercher sur le net).

### Shadow Mapping **

Le shadow mapping consiste à utiliser la depth map d'un rendu du point de vue de la lumière pour calculer des ombres portées.

Le shadow mapping d'une lumière directionelle n'est pas très compliqué à implémenter et les deux tutos suivants peuvent être suivi:

- https://celeborn2bealive.github.io/openglnoel/docs/gold-04-shadow-mapping-01-introduction
- https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping

Si vous avez en plus choisi de faire le sujet "many lights", vous pouvez implémenter le shadow mapping sur les points lights. Cela augmente la difficulté du sujet à 3 étoiles. Voici un tuto:

- https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows

### Deferred Rendering et Post Processing ***

Le deferred rendering est une technique consistant à splitter le rendu en deux parties: une partie "geometry", qui se contente de faire un rendu des propriétés de geometrie et materiaux et objets dans des textures intermédiaires (le GBuffer), et une partie "lighting" qui utilise le GBuffer pour faire le rendu final.

Cette technique n'est en soit pas très compliquée à implémenter, c'est pourquoi ce sujet comporte aussi une partie Post Processing, qui exploite les textures intermédiaires calculées par le deferred rendering pour rajouter des effets à l'image.

Le deferred rendering peut être implémenté en suivant les tuto:

- https://celeborn2bealive.github.io/openglnoel/docs/gold-03-deferred-renderer-01-pipeline
- https://learnopengl.com/Advanced-Lighting/Deferred-Shading

Je conseille également de permettre à l'utilisateur de switcher entre forward rendering (le rendu comme on l'a fait jusqu'a présent) et deferred rendering. Cela vous obligera à garder votre ancien code fonctionnel.

Il faudra qu'il soit possible d'afficher les textures du GBuffer via une option de la GUI.

Pour ce qui est du post processing, il faudra au moins implémenter le Screen Space Ambiant Occlusion (SSAO), en suivant ce tuto:

- https://learnopengl.com/Advanced-Lighting/SSAO

Après ça vous etes libre d'implémenter d'autres effets post process comme:
- le Bloom https://learnopengl.com/Advanced-Lighting/Bloom
- le Depth of field en screen space (je n'ai pas de tuto mais ça doit se trouver facilement)
- la detection et l'affichage de contours (pareil)

### PBR Lighting ***

Le lighting PBR (physically based rendering) permet d'exploiter toute la puissance du modèle de materiaux glTF et ainsi obtenir un rendu vraiment réaliste.

Les deux tutos suivant donnent plus de détails sur le modèle de matériau PBR que l'on a implémenté, je conseille de les lire avant de passer à l'applicatif:
- https://learnopengl.com/PBR/Theory
- https://learnopengl.com/PBR/Lighting

C'est globalement le même modèle de materiaux que celui de glTF, avec peut être quelques différences mais rien de majeur.

La partie à implémenter est détaillée par les tutos suivants:
- https://learnopengl.com/PBR/IBL/Diffuse-irradiance
- https://learnopengl.com/PBR/IBL/Specular-IBL