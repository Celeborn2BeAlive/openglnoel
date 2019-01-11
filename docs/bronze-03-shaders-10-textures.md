---
id: bronze-03-shaders-10-textures
title: Textures
---

L'objectif est à présent d'appliquer une texture aux triangles. Téléchargez le fichier [triforce.png](assets/triforce.png) et placez le dans un répertoire nommé assets/textures à la racine de votre répertoire de TP.

## Charger l'image

La fonction **loadImage** déclarée dans le fichier **glimac/Image.hpp** vous permet de charger une image depuis le disque. Cette fonction renvoit un **std::unique_ptr<Image>** (pointeur sur une image qui désalloue sa mémoire tout seul dans son destructeur).

<span class="badge todo"></span> En utilisant cette fonction, chargez la texture de triforce (utilisez le chemin absolu vers le fichier). Vérifiez que le chargement a bien réussi en testant si le pointeur renvoyé vaut NULL. Le chargement de la texture doit être fait avant la boucle de rendu (par exemple juste avant le chargement des shaders).

<span class="badge warning"></span> Dans un vrai moteur on n'utilise pas des chemins absolus vers les fichier: on s'arrange pour que les assets soient situés relativement par rapport à l'executable et on les charge en utilisant le chemin de l'executable (c'est ce qui est fait pour les shaders dans votre code). J'ai décidé de ne pas faire copier au CMake les assets à coté de l'executable car ces derniers peuvent être gros (comparé à un shader) et votre place en mémoire est limitée sur les machine de la fac.

## Créer un texture object OpenGL

Il faut ensuite envoyer la texture à la carte graphique. Pour cela OpenGL propose les texture objects.

<span class="badge todo"></span> En utilisant la fonction [glGenTextures](http://docs.gl/gl3/glGenTextures), créez un nouveau texture object.

<span class="badge todo"></span> A l'aide de la fonction [glBindTexture](http://docs.gl/gl3/glBindTexture), bindez la texture sur la cible **GL_TEXTURE_2D**.

<span class="badge todo"></span> Utilisez ensuite la fonction [glTexImage2D](http://docs.gl/gl3/glTexImage2D) pour envoyer l'image à la carte graphique afin qu'elle soit stockée dans votre texture object. Pour cela il faut utiliser les membres suivants de la classe Image: **pImage->getWidth()** pour obtenir la largeur, **pImage->getHeight()** pour obtenir la hauteur et **pImage->getPixels()** pour obtenir le tableau de pixels. La fonction **glTexImage2D** prend également en paramètre des formats (internalFormat et format), passez lui pour ces deux paramètres la constante **GL_RGBA**. Le paramètre **type** doit être **GL_FLOAT** car la classe Image stocke ses pixels en flottants. Enfin les paramètres **level** et **border** doivent être mis à 0.

<span class="badge todo"></span> Afin de pouvoir utiliser une texture, il faut spécifier à OpenGL des filtres que ce dernier appliquera lorsque 1) plusieurs pixels à l'écran sont couvert par un pixel de texture et 2) un pixel à l'écran couvre plusieurs pixels de texture. Rajoutez les lignes suivantes:

```cpp
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
```

Ces deux lignes permettent d'appliquer un filtre linéaire dans les deux cas cités plus haut. Référez vous à la documentation si vous êtes interessé par d'autres filtres.

<span class="badge todo"></span> Débindez la texture en utilisant [glBindTexture](http://docs.gl/gl3/glBindTexture) avec 0 passé en deuxième paramètre.

<span class="badge todo"></span> A la fin du programme, ajoutez un appel à [glDeleteTextures](http://docs.gl/gl3/glDeleteTextures) afin de détruire le texture object.

## Spécifier les coordonnées de texture

Jusqu'a présent nous avions mis les coordonnées de texture des vertex à \\((0,0)\\). Il faut les changer afin que chaque vertex soit associé au bon pixel dans la texture. Voici un schéma indiquant les coordonnées de texture à associer à chaque vertex:

![Texture schema](/openglnoel/img/texture_shema.png)

Le coin haut-gauche a pour coordonnées \\((0,0)\\) et le coin bas-droit \\((1,1)\\) (quelque soit les dimensions de l'image d'entrée).

<span class="badge todo"></span> Dans le tableau de sommets, modifiez les coordonnées de texture de chacun des sommets à partir du schéma ci dessus.

## Utiliser la texture dans le shader

Une texture s'utilise dans un fragment shader en utilisant une variable uniforme de type **sampler2D**.

<span class="badge todo"></span> Ajoutez une variable uniforme nommée **uTexture** dans votre fragment shader.

Il est ensuite possible de lire dans la texture en utilisant la fonction GLSL **texture(sampler, texCoords)**. Le premier paramètre est le sampler2D (**uTexture** dans notre shader) et le deuxième paramètre les coordonnées de texture du fragment. Ces dernières doivent être récupérée en entrée du fragment shader depuis le vertex shader (comme nous faisions au TP précédent pour la couleur et la position).

<span class="badge todo"></span> Faite en sorte que la couleur affichée par le fragment shader soit celle lue depuis la texture à la bonne position. Attention: la fonction **texture** renvoit un **vec4**, il faut donc le transformer en **vec3** si votre variable de sortie du fragment shader est de type **vec3**.

## Spécifier la valeur de la variable uniforme

<span class="badge todo"></span> Dans le code de l'application, récupérez la location de la variable uniforme en utilisant **glGetUniformLocation**

Les sampler GLSL sont en réalité des entiers. Il faut les remplir à l'indice de l'unité de texture sur laquelle est branchée la texture voulue. Nous verrons plus tard les unités de texture, pour l'instant nous en utilisons une sans nous en rendre compte: l'unité de texture 0. Il faut donc remplir la variable uniforme avec la valeur 0 en utilisant la fonction **glUniform1i**.

<span class="badge todo"></span> Dans la boucle de rendu, avant l'appel à **glDrawArrays**: bindez la texture sur la cible **GL_TEXTURE_2D**, puis fixez la valeur de la variable uniforme **uTexture** à 0. Après l'appel à **glDrawArrays**, débindez la texture. Testez votre programme: les triangles doivent à présent être texturés.