---
id: silver-04-lighting-04-aller-plus-loin
title: Aller plus loin
---

Pour créez un moteur de rendu plus complet il faudrait basiquement faire deux choses:

Gérer les matériau avec des textures, comme mentionné dans l'introduction. Cela consiste simplement à lire les coefficient \\(K_d\\), \\(K_s\\) et \\(shininess\\) dans des textures. Pour accéder à 3 textures en même temps il faut faire du multi-texturing.

Pouvoir envoyer plusieurs lumières. Pour cela on peut utiliser des tableaux d'uniformes (non couvert sur ces TPs, mais vous pouvez le faire pour le projet). Une autre solution est d'utiliser des textures 1D pour stocker les lumières (une texture 1D est équivalent à un tableau simple), ce qui permet d'envoyer plus de lumières. Enfin une meilleure option de faire du deferred shading qui permet de gagner en performance lorsque le nombre de lumières augmente.