---
id: bronze-01-philogl-01-introduction
title: Introduction
---

Si vous avez fait déjà fait de l'OpenGL 2, il est nécessaire que vous compreniez pourquoi vous devez tout oublier (ou presque) et qu'est ce qui va changer. OpenGL 3 est souvent jugé plus compliqué qu'OpenGL 2. En réalité ce qui est plus compliqué c'est surtout de s'y mettre :p En effet, la quantité de code necessaire pour obtenir un simple triangle coloré est impressionante, ce qui a de quoi décourager. Néanmmoins, une fois ce code mis en place il reste généralement identique d'une application à l'autre, d'où l'intéret de coder une bibliothèque pour factoriser son code en parallèle des TDs.

## Qu'est ce que OpenGL ?

OpenGL est une API (Application Programming Interface) destinée au rendu par rasterisation. Ca fait beaucoup de mots compliqués, on va donc détailler un peu tout ça !

## Cartes graphiques et pilotes

Une API est simplement la spécification d'une liste de types, de fonctions et de constante ainsi qu'unne description de l'effet attendu pour chaque fonction. Cela signifie qu'OpenGL n'est pas une implémentation. N'importe qui peut décider de coder l'ensemble de l'API OpenGL. En particulier les constructeurs de carte graphique le font généralement et proposent un driver. Le driver (pilote en français) est un composant logiciel permettant de piloter (comme son nom l'indique) le materiel du constructeur, c'est à dire la carte graphique. Ce driver sert également de bibliothèque pour les développeurs que nous sommes. La carte graphique (également appelée GPU pour Graphical Processing Unit) permet d'accélerer les calculs de rendu en fournissant du matériel dedié. Il est néammoins possible d'utiliser une implémentation logicielle (c'est à dire qui s'éxecute sur CPU) de OpenGL comme par exemple MesaGL.

## Rasterisation

La rasterisation est une méthode de rendu consistant à transformer les primitives 3D (triangles, lignes et points) en pixels (appelés fragments). OpenGL n'est pas une bibliothèque 3D. Ou en tout cas ne l'est plus, depuis OpenGL 3. Comme vous le verrez OpenGL 3 n'a que peu de connaissances relatives à la notion de 3D. C'est à vous, grace au pipeline programmable de coder tout ce qui concerne la 3D. C'est ce qui fait la grande force d'OpenGL: l'ensemble est très modulaire et peut être utilisé pour faire de la 2D, de la 3D ou même encore des taches qui n'ont rien à voir avec le rendu mais qui demandent une grande puissance de calcul parallèle.

## Le calcul parallèle sur GPU

En effet, l'avantage d'utiliser le GPU est d'avoir accès à un processeur spécialement dedié au calcul parallèle. Un GPU est constitué de milliers de coeurs pouvant éxecuter le même code au même moment. Chaque coeur n'est pas très puissant (moins qu'un coeur CPU en tout cas), mais c'est leur nombre qui fait leur force. Ainsi, faire plein de petites taches en parallèle sur des coeurs moins puissant permet généralement d'aller plus vite qu'en les éxecutant séquentiellement sur un processeur très puissant.

## Direct3D, la concurrence

Direct3D, maintenu par Microsoft, est l'une des rares API concurrente à OpenGL. Tout ce qui a été dit plus haut est valable pour Direct3D. Cette dernière a eu à une époque beaucoup plus de popularité qu'OpenGL. Les choses sont en train de changer, surtout depuis que la plate forme de jeu Steam a été porté sous Linux :p

## Vulkan, le niveau superieur