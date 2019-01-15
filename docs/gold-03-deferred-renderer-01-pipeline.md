---
id: gold-03-deferred-renderer-01-pipeline
title: Pipeline de Rendu
---

## C'est quoi le problème ?

Un problème bien connu du *forward shading* est le traitement par le fragment shader de fragments qui ne seront pas visible à l'écran.
Tous les fragment occultés par d'autre fragments sont traités car le fragment shader peut potentiellement modifier la profondeur des fragments et donc les rendre visible.
Ainsi, si le fragment shader est complexe, tous ces calculs inutiles peuvent devenir couteux.

En plus de cela, il est assez difficile en une seule passe de rendu de pouvoir optimiser son algorithme pour faire moins de calcul.
Un exemple simple est l'utilisation d'un grand nombre de lumières n'émettant que dans un volume réduit de la scène.
Ce type de lumière affecte assez peu de pixels.
Avec un forward renderer, il n'y a que dans le fragment shader qu'on peut s'appercevoir qu'une lumière donnée n'affecte pas le fragment, c'est déjà trop tard.
En ayant accès à plus d'information sur la géométrie visible par la caméra, il est possible d'affecter rapidement des listes de lumières à des tiles de pixel et ainsi de passer moins de temps globalement dans le fragment shader.

Le deferred rendering est un autre algorithme de rendu permettant de palier à ces problèmes.

## Le Deferred Shading

Le principe du deferred est de découpler le traitement de la géométrie (vertex buffer + rasterisation) du traitement des fragments (shading dans le fragment shader).

Une première passe de rendu dessine donc tous les objets de la scène, mais plutot que de les illuminer dans le fragment shader, elle écrit les informations de chaque fragment dans des textures.
Ces informations sont celles utilisées pour le lighting: position, normale et texCoords du fragment, mais également paramètres de matériaux (ambiant, diffus, glossy et shininess).
Comme ces informations sont majoritairement de nature géométrique, l'ensemble des textures de sortie est généralement appelé **GBuffer** et cette première passe de rendu s'appelle la **Geometry Pass**.

Une fois la première passe accomplie et le GBuffer remplit, on effectue la **Shading Pass**, qui consiste à utiliser le GBuffer pour illuminer chaque pixel.
Au cours de cette passe, on dessiner simplement un quad couvrant l'écran, le vertex shader ne fait donc quasiment rien.
Au contraire, le fragment shader lit dans les textures du GBuffer les informations du pixel courant (qui ont été enregistré pendant la geometry pass) et calcule la couleur finale à partir de ces infos et des lumières de la scène.

L'image ci-dessous montre en haut un rendu final, résultat de la Shading Pass appliquée au GBuffer donc les textures sont montrés en bas:
![GBuffer](/openglnoel/img/gbuffer_1.png)

A noter qu'il y énormément de manière de remplir un GBuffer en fonction des performances voulues.
Pour ces TPs, on se contentera d'écrire tous les informations dont on a besoin pour le lighting, sans chercher à optimiser.

## Liens

- [Explication des deux approches](https://gamedevelopment.tutsplus.com/articles/forward-rendering-vs-deferred-rendering--gamedev-12342)
- [Tuto OpenGL Dev](http://ogldev.atspace.co.uk/www/tutorial35/tutorial35.html)
