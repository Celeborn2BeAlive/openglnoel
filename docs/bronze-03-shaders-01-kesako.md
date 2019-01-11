---
id: bronze-03-shaders-01-kesako
title: Shaders, kesako ?
---

Vous avez appris à envoyer des données à la carte graphique afin de dessiner des formes simples. L'objectif est maintenant d'apprendre à coder des shaders et les brancher à votre pipeline graphique afin de personnaliser l'algorithme de rendu implémenté par OpenGL.

## Qu'est ce qu'un shader ?

Un shader est un petit programme qui s'execute sur la carte graphique. Votre GPU implémente un pipeline de rendu dont certains stages sont laissés à votre responsabilité. Si vous ne les implémentez pas, un comportement par défaut les remplace (pas de transformations, couleur blanche). Cette possibilité de coder certains stages du pipeline offre une puissance considérable. Basiquement c'est ce qui fait la différence entre des graphismes Playstation 2 et des graphismes Playstation 3: le GPU programmable.

## Quel language de programmation ?

Puisqu'un shader est un programme, il faut le coder avec **un language de shading**. Il existe plusieurs languages de shading, les plus connus étant **HLSL** (qui va avec Direct3D, donc Microsoft) et **GLSL** (qui va avec OpenGL). Nous allons bien évidemment utiliser le dernier.

## Les Vertex Shaders

Un **Vertex Shader (VS)** prend en entrée les attributs d'un sommet et a pour rôle principal d'appliquer une transformation géométrique pour **projeter ce sommet à l'écran**. OpenGL utilise ensuite les positions projetées de tous les sommets pour rasteriser les triangles projetés. La **rasterisation** consiste simplement à transformer les triangles en **fragments** (un fragment est un pixel).

En plus de cela le VS peut fournir en sortie des valeurs qui seront utilisées par le fragment shader. Ces valeurs sont interpolées sur les fragments des triangles rasterisés afin de créer un dégradé des valeurs. L'exemple le plus parlant est celui des couleurs: on observe bien dans l'exemple du triangle en couleur que les couleurs sont interpolées sur le triangle. Il est possible de faire de même avec d'autre types de valeurs qui nous seront utiles par la suite (normales ou coordonnées de texture par exemple).

![Vertex Shader](/openglnoel/img/vertex_shader.svg)

## Les Fragment Shaders

Un **Fragment Shader (FS)** prend en entrée un fragment calculé par la rasterisation (un triangle est transformée en plein de fragments). En terme de programmation, un fragment est représenté par l'ensemble des valeurs de sortie du VS. Le FS a pour rôle de calculer la couleur finale du pixel correspondant au fragment en utilisant les valeurs interpolées par la rasterisation.

Le schéma suivant illustre le processus de rasterisation appliqué sur un triangle et l'ensemble des fragments qui en résulte:

![Vertex and Fragment](/openglnoel/img/vertfrags.png)

La couleur de chaque fragment est obtenue par interpolation des couleurs des 3 sommets. C'est la couleur interpolée qui arrive en entrée du fragment shader.

Lorsqu'on fait du rendu 3D le FS doit calculer la couleur finale du fragment à partir des données géométriques interpolées (position, normale, coordonnées de texture) et des lumières de la scène.

![Fragment Shader](/openglnoel/img/fragment_shader.svg)

## Les autres types de shader

Nous n'utiliserons que les VS et les FS mais il existe d'autres types de shader. Voici la liste pour votre culture:

- Geometry Shader: Permet de créer à la volée de nouvelles primitives géométrique (on peut par exemple générer des cubes à partir de simples points).
- Tesselation Shader: Permet de tesseller dynamiquement les primitives d'entrée, c'est à dire créer de nouveaux points sur les triangles à des endroits bien précis. Très utile pour ajouter du niveau de détail lorsque la caméra se rapproche des objets.
- Compute Shader: Permet de faire du calcul parallèle sur GPU (comme en Cuda pour ceux qui connaissent). Ce type de shader n'est même pas lié au pipeline et peut être lancé à n'importe quel moment.

Contrairement aux VS et FS, il n'est pas obligatoire d'utiliser ces trois autres types de shader pour faire de la 3D simple.

## A retenir
Deux points importants à retenir:

- Un vertex shader traite un vertex provenant du CPU (envoyé par votre code C++) et produit une position projetée à l'écran.
- Un fragment shader traite un fragment généré par le rasterizer et produit une couleur à afficher à l'écran dans le pixel correspondant au fragment.

<span class="badge todo"></span> Afin de conserver le code du TP1 tel quel, dupliquez le répertoire **TPtemplate** et renommez le **TP2**. Travaillez dedans pour ce TP.