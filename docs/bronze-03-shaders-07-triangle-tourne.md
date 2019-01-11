---
id: bronze-03-shaders-07-triangle-tourne
title: Un triangle qui tourne
---

Jusqu'ici nos scènes ont été statique. Nous allons utiliser la notion de variable uniforme pour envoyer au shader le temps courant et faire tourner le triangle en fonction de ce temps.

## Les variables uniformes

Une variable uniforme est une variable d'entrée d'un shader **qui reste constante pour tous les sommets d'un appel de dessin (draw call)**.

Un draw call est simplement un appel à une fonction de la forme **glDraw\***, comme par exemple **glDrawArrays** que vous utilisez dans votre boucle de rendu.

Lorsqu'on appelle une de ces fonctions, un ensemble de sommets est dessiné par la carte graphique sous la forme de primitives, en utilisant les shaders actifs.

Avant de faire un draw call, il est possible d'envoyer des valeurs à nos shaders à l'aide de variables uniformes. Ces valeurs resteront constantes pour tous les sommets dessinés par le draw call. Après le draw call, on peut modifier ces valeurs et faire un nouveau draw call pour dessiner un objet paramétré par les nouvelles valeurs.

L'exemple le plus courant est celui des transformations. Il est possible d'envoyer une matrice sous la forme de variable uniforme qui sera appliquée par nos shaders à tous les sommets. On peut ensuite dessiner plusieurs fois le même objet à des positions différentes simplement en changeant la matrice entre chaque appel de dessin.

Un autre exemple simple: les textures. On peut changer la ou les textures courantes avec des variables uniformes. Ainsi on peut dessiner le même objet plusieurs fois mais avec une apparence différente.

## Définir une variable uniforme

<span class="badge todo"></span> Dans votre vertex shader (**tex2D.vs.glsl**), ajoutez avant le main la ligne:

```cpp
uniform float uTime;
```

Cette ligne définie la variable uniforme **uTime**. Comme son nom l'indique, nous allons l'utiliser pour envoyer le temps écoulé depuis le début de l'application à notre shader.

<span class="badge todo"></span> Récupérez votre fonction **mat3 rotate(float a)** écrite pour les shaders du TP précédent. Dans le main du shader, utilisez cette fonction pour construire une matrice de rotation avec pour angle la valeur de la variable **uTime**. Utilisez ensuite cette matrice pour transformer le sommet en entrée du shader (référez vous au TP précédent si vous avez oubliez ces notions).

> Par convention, on préfixe les variables uniforme par un "u" pour les réperer rapidemment.

## Modifier la variable uniforme depuis l'application

Placez vous à présent dans le **main** de l'application (code C++).

Chaque variable uniforme définie dans un programme GLSL (vertex shader ou fragment shader) possède une **location**, qui est simplement un entier allant de zéro au nombre total de variables uniformes moins un.

Cette location doit être récupérée pour pouvoir modifier la valeur de la variable présente dans le shader. Pour cela il faut utiliser la fonction **glGetUniformLocation**.

<span class="badge todo"></span> Juste après l'activation du programme GLSL (ligne **program.use()** normalement), utilisez la fonction **GLint glGetUniformLocation(GLuint program, const GLchar *name)** pour récuperer la location de votre variable uniforme. Le premier argument de cette fonction est l'identifiant OpenGL du programme, que vous pouvez obtenir en utilisant la méthode **getGLId()** de la classe **Program**.

<span class="badge doc"></span> [glGetUniformLocation](http://docs.gl/gl3/glGetUniformLocation)

Il est ensuite possible de modifier la valeur de la variable uniforme en utilisant les fonctions ayant la forme **glUniform\***. Il existe une fonction pour chaque type GLSL possible.

<span class="badge doc"></span> [glUniform](http://docs.gl/gl3/glUniform)

Par exemple pour modifier une uniforme de type **float**, il faut utiliser la fonction **glUniform1f**. Pour le type **vec3**, il faut utiliser **glUniform3f**.

Avant d'appeler cette fonction, il faut avoir récupérée la location (car c'est le premier argument à passer à la fonction), et il faut que le programme définissant l'uniforme soit activé.

<span class="badge todo"></span> Dans un premier temps, avant le main, utilisez la fonction pour fixer l'uniforme **uTime** à la valeur 45. Essayez le programme, votre triangle devrait s'afficher tourné de 45°.

Si ça ne fonctionne pas, plusieurs choses à vérifier:

- La fonction **glGetUniformLocation** doit vous avoir renvoyé une valeur non-négative. Si ce n'est pas le cas, alors soit vous n'avez pas définie la variable **uTime** dans le vertex shader, soit vous ne l'utilisez pas.
- Vérifiez que vous appliquez bien la rotation à vos sommets dans le vertex shader.
- Vérifiez que le programme est bien activé au moment de l'appel à **glUniform1f** (il faut le faire après **program.use()**).

## Tu tournes !

L'intérêt des variables uniformes, c'est de les changer à chaque tour de boucle. Il faut la changer avant l'appel à **glDrawArrays**, afin que la modification soit prise en compte.

<span class="badge todo"></span> En incrémentant une variable (de type float) à chaque tour de boucle, modifiez la valeur de la variable uniforme **uTime** avant l'appel de dessin (glDrawArrays). Testez votre programme, le triangle devrait s'animer et tourner sur lui même.

> <span class="badge warning"></span> La vitesse de rotation est determinée par la valeur d'incrémantation appliquée à chaque tour de boucle. Cette vitesse dépend donc des performances de notre application. Si on voulait faire les choses proprement, on récupererait le temps réel écoulé depuis le dernier tour et en fonction d'une valeur de vitesse spécifiée on modifierai l'angle.