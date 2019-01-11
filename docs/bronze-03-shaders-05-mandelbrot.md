---
id: bronze-03-shaders-05-mandelbrot
title: Mandelbrot
---

Nous allons à nouveau faire le rendu d'une texture procédurale, sur toute la fenêtre cette fois. L'objectif est de coder le rendu de [la fractale de Mandelbrot](https://www.wikiwand.com/fr/Ensemble_de_Mandelbrot). Pour cet exercice, reprenez le fichier template de base (TPtemplate.cpp), copiez le et renommez le.

![Mandelbrot](/openglnoel/img/mandelbrot.png)

## Dessiner toute la fenêtre

<span class="badge todo"></span> Première chose à faire: dessiner un quad qui occupe tout l'ecran (de -1 à 1 sur chacun des axes). Nous avons déjà dessiné un quad dans un exercice précédent donc vous ne devriez pas avoir de problème. N'envoyez que des positions dans le VBO (pas de couleur). Testez votre programme avant de passer à la suite (la fenêtre doit être toute blanche).

## Mettre en place les shaders

<span class="badge todo"></span> Créez deux shaders **mandelbrot.vs.glsl** et **mandelbrot.fs.glsl**. Codez les pour le moment de manière à afficher du rouge à la place du blanc. Depuis l'application chargez les. Testez votre application pour vérifier que la fenêtre est bien rouge.

<span class="badge todo"></span> Comme pour certains des exercices précédents, nous allons avoir besoin de la position du fragment en entrée dans le fragment shader. Faites en sorte qu'elle soit passée du vertex au fragment shader. Pour vérifier que c'est correct affichez cette position comme si c'était une couleur (le x,y devient le r,g de la couleur de sortie).

## La fractale de Mandelbrot

La fractale de Mandelbrot est un ensemble noté \\(M\\) de nombres complexes respectant une certaine propriété. L'idée pour dessiner cet ensemble est d'identifier chaque fragment à un nombre complexe. En effet, les nombres complexes peuvent être placés sur un plan, et donc dessinés en 2D. Nous dessinerons en noir les fragments donc le nombre complexe associé appartient à l'ensemble de mandelbrot et en blanc les autres.

Commençons par la définition formelle de l'ensemble \\(M\\). Pour chaque nombre complexe \\(c=a+ib\\), on définit une suite récurrente d'autres nombre complexe:

- \\(z_0(c)=c\\)
- \\(z_{k+1}(c)=z_k(c)^2+c\\)

Prenons par exemple le nombre c=2+i et calculons les premiers termes de la suite (rappel: \\(i^2=−1\\)):

$$
z_0(c)=2+i
$$

$$z_1(c)=z_0(c)^2+c=(2+i)^2+(2+i)=(2^2+2×2×i+i^2)+(2+i)\\\\=4+4i−1+2+i=5+5i$$

$$z_2(c)=z_1(c)^2+c=(5+5i)^2+(2+i)=(5^2+2×5×i+5^2i^2)+(2+i)\\\\=25+10i−25+2+i=2+11i$$

Pour chaque nombre complexe c, on s'interesse au comportement de la suite \\((z_n(c))_{n \geq 0}\\). Si tous les elements de la suite sont situés dans le disque de rayon 2 centré sur l'origine \\((0,0)\\), alors \\(c\\) appartient à \\(M\\). Si des elements de la suite sont situés à l'exterieur de ce disque, alors \\(c\\) n'appartient pas à \\(M\\). Cela revient à vérifier si la condition \\(\forall n \in \mathbb{N}, |z_n(c)| \leq 2\\) est respectée (rappel: \\(|z_n(c)|\\) est le module du nombre complexe \\(z_n(c)\\) et représente la distance entre l'origine et le point associé au nombre complexe).

Comment convertir tout ça en code ? c'est simple: dans le fragment shader on récupère la position \\((x,y)\\) du fragment (obtenue en entrée grace au vertex shader). A partir de cette position on calcule les termes de la suite \\(z_n(x+iy)\\) pour \\(n\\) variant de \\(0\\) à un entier maximal \\(N_{max}\\) fixé (dans une boucle). Si le module d'un des termes est supérieur à 2, on quitte la boucle et on sort la couleur blanche. Si ce n'est pas le cas, on sort la couleur noire.

Concretement, on n'affiche qu'une approximation de la fractale. Plus \\(N_{max}\\) est elevé, meilleure est l'approximation (et plus long est le calcul).

<span class="badge todo"></span> Le calcul des termes de la suite requiert une multiplication complexe (le terme précédent élevé au carré). La multiplication des **vec2** en GLSL ne correspond pas à la multiplication complexe (car il n'y a pas de prise en compte de i). Dans le fragment shader, ecrivez une fonction **vec2 complexSqr(vec2 z)**$** qui calcule le carré du nombre complexe **z.x+i\*z.y** représenté par **z**. Comprenez bien que i n'apparait pas dans la fonction, ce n'est qu'une entitée mathématique. Le calcul doit d'abord être posé sur papier, en appliquant la propriété \\(i^2=−1\\). A partir de ça on renvoit dans la fonction un **vec2** ayant en x la partie réelle et en y la partie imaginaire du résultat.

<span class="badge todo"></span> Implantez dans le main du fragment shader l'algorithme décrit plus haut pour calculer la couleur du fragment en fonction de son appartenance à l'ensemble de mandelbrot. N'oubliez pas d'utiliser votre fonction **complexSqr** pour calculer chacun des termes de la suite ! Pour calculer le module des termes, vous pouvez utiliser la fonction GLSL length qui calcule la longueur d'un vecteur (c'est équivalent au module).

<span class="badge todo"></span> Une fois que vous avez la fractale en noir et blanc, il est possible d'ameliorer facilement le rendu en affichant une couleur qui dépend du nombre de tour de boucle qui se sont écoulés avant que la condition soit rompue. Imaginons que le test échoue au tour de boucle **j**, vous pouvez afficher comme couleur la valeur: **vec3(float(j) / Nmax)**; Avec un peu d'imagination vous pouvez même faire plus joli :)