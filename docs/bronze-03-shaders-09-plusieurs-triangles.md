---
id: bronze-03-shaders-09-plusieurs-triangles
title: Plusieur triangles
---

Un autre intérêt des variables uniformes est de pouvoir dessiner un objet plusieurs fois avec des paramètres différents. Ainsi on utilise qu'un seul couple VBO-VAO pour l'objet, mais on fait plusieurs appels de dessin en modifiant entre chaque appel les variables uniformes.

## Des triangles

<span class="badge todo"></span> Dans la boucle de rendu, dessinez 4 fois le triangle. En modifiant correctement la variable uniforme **uModelMatrix** avant chaque appel à **glDrawArrays**, faites en sorte qu'une triangle soit placé au centre de chaque quart de l'écran (utilisez la translation).

<span class="badge todo"></span> Avec la multiplication matricielle, faites en sorte que les triangles soient dessiné avec un quart de leur taille initiale.

<span class="badge todo"></span> Faites en sorte que chaque triangle tourne sur lui même. Deux des triangles doivent tourner dans un sens, les deux autres dans le sens inverse.

## Et des couleurs !

<span class="badge todo"></span> Ajoutez dans le fragment shader une nouvelle variable uniforme **uniform vec3 uColor**;. Dans le main du shader, fixez la couleur du fragment en utilisant cette variable.

<span class="badge todo"></span> Dans le fragment shader, récupérez la location de la nouvelle variable uniforme uColor. Faites ensuite en sorte d'afficher chaque triangle avec une couleur différente.

## Et... des maths :D

<span class="badge todo"></span> Petit exercice pratique sur les matrices: faites en sorte que les triangles tournent tous autour du centre de l'écran (en plus de tourner sur eux même). Attention à l'ordre des multiplications de matrices.