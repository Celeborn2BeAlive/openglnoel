---
id: bronze-03-shaders-08-matrices
title: Envoyer des matrices
---

Votre triangle doit tourner correctement mais le code n'est pas efficace: nous calculons la matrice de rotation dans le vertex shader. Ce calcul est fait pour chacun des 3 sommets, alors que c'est la même matrice pour chacun ! Lorsque l'on a que 3 sommet ça va, mais si on fait ce calcul sur 1 million de sommets, la perte de performance risque de se sentir. De manière générale, on calcule les matrices dans l'application et on les envoit au shader. C'est ce que nous allons faire.

## Les matrices sur CPU

La bibliothèque **glm** permet d'utiliser des matrices grace aux types **glm::mat2**, **glm::mat3** et **glm::mat4** (ainsi que d'autres types pour les matrices rectangulaire). Elle propose également des fonctions pour construire les matrices de transformation standard. Malheuresement ces fonctions renvoient des matrices 4x4 (adaptés à la 3D). Nous les utiliserons dans les prochains TPs. Pour le moment nous avons besoin de matrices 3x3. Vous allez donc re-coder, dans votre code C++, les fonctions pour créer des matrices que vous avez déjà implanté dans le shader.

<span class="badge todo"></span> Ecrivez les fonctions **translate**, **scale** et **rotate**. Chacune doit renvoyer une matrice de type **glm::mat3**. Repenez le code des fonctions équivalentes codées en GLSL pour vous aider.

## Modifier le shader

<span class="badge todo"></span> Modifiez le shader: la variable uniforme en entrée doit maintenant être de type **mat3**. Renommez la **uModelMatrix**.

<span class="badge todo"></span> Modifiez le main du shader pour appliquer directement la matrice **uModelMatrix** à la position du sommet d'entrée.

## Modifier l'application

<span class="badge todo"></span> Dans votre boucle de rendu, utilisez votre fonction **rotate** pour construire une matrice de rotation à partir du temps écoulé. Utilisez ensuite la fonction **glUniformMatrix3fv** pour envoyer la matrice au shader. Attention aux paramètres de cette dernière fonction: le paramètre count est le nombre de matrices que l'on envoit (donc 1) et le paramètre transpose doit être fixé à GL_FALSE (voir la doc). Pour passer le pointeur vers la matrice, utilisez la fonction **glm::value_ptr** (il faut pour cela inclure le fichier **<glm/gtc/type_ptr.h>**).

<span class="badge doc"></span> [glUniform](http://docs.gl/gl3/glUniform)

<span class="badge todo"></span> Testez votre application, le triangle doit toujours tourner.