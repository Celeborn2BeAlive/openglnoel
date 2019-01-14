---
id: silver-02-3d-pratique-05-dessiner-terre
title: Dessiner la terre
---

Nous allons utiliser des textures pour donner à la planète l'apparence de la terre. La classe **glimac::Sphere** calcule automatiquement des coordonnées de textures qui sont reçuent par le shader. Il suffit donc de charger et binder correctement une texture de terre puis d'y accéder correctement depuis le fragment shader pour avoir une apparence correcte.

<span class="badge todo"></span> Téléchargez les texture [EarthMap.jpg](assets/EarthMap.jpg) et [MoonMap.jpg](assets/MoonMap.jpg) et placez les dans le répertoire **textures** de votre répertoire de TP.

<span class="badge todo"></span> Créez un nouveau shader **tex3D.fs.glsl**. Dans celui ci, créez une variable uniforme de type **sampler2D** et utilisez les coordonnées de texture en entrée pour obtenir la couleur de sortie en lisant dans la texture. Au besoin relisez le TP sur les textures pour vous aider.

<span class="badge todo"></span> Dans le code de l'application chargez la texture EartMap.jpg. Créez un texture object OpenGL à partir de l'image SDL et bindez la correctement pour qu'elle soit affichée lors du rendu. Testez le programme. Faites ensuite en sorte que la terre tourne sur elle même, mais pas les lunes !

<span class="badge todo"></span> Un problème apparait normalement: vos lunes ont également l'apparence de la terre. Afin de modifier cela, il suffit de modifier la texture bindée avant de dessiner les lunes. Chargez la texture MoonMap.jpg, créez un texture object OpenGL associé et utilisez ce dernier pour dessiner les lunes.

![Terre](/openglnoel/img/terre.png)