---
id: bronze-02-hello-triangle-05-dessiner-quad
title: Dessiner un quad
---

A partir de maintenant, à vous de vous débrouiller. Je vous indique ce qu'il faut faire sans le blabla qui va avec.

Le but de cet exercice est de dessiner un quad au centre de l'écran (avec des couleurs) à la place du triangle. Le quad doit aller de -0.5 à 0.5 sur chacun des deux axes.

<span class="badge todo"></span>  Commencez par dessiner le quad sur une feuille et découpez le en deux triangles. Identifiez la position de chaque sommet.

<span class="badge todo"></span>  Dessiner un quad, c'est juste dessiner deux triangles, donc 6 sommets. En utilisant les positions identifiées sur papier, modifiez votre code pour dessiner le quad. Il faut: modifier le tableau de sommet, l'appel à glBufferData (pour la taille du tableau passée en paramètre) et l'appel à glDrawArrays (dans la boucle de rendu) de manière à dessiner 6 sommets au lieu de 3.