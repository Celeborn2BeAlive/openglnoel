---
id: silver-06-course-01-opengl-context
title: Contexte OpenGL
---

OpenGL est une [API](https://www.wikiwand.com/en/Application_programming_interface) permettant d'effectuer du calcul parallèle sur GPU, principalement orienté sur la rasterisation de primitives simples (points, lignes et triangles) afin de les afficher dans une image.

OpenGL **n'est pas** une bibliothèque, mais simplement une [spécification](https://www.opengl.org/registry/doc/glspec45.core.pdf) de fonctions permettant l'utilisation d'effectuer différent de taches de manière asynchrones, souvent centrées autour de la programmation graphique.

[Khronos](https://www.khronos.org/) est le groupe chargé d'établir la spécification de chaque version d'OpenGL. Plusieurs [entreprises/universités/personnes](https://www.khronos.org/members/list) font partie de ce groupe afin de faire avancer la spécification et s'adapter à l'évolution de l'architecture du hardware.

Les constructeurs de cartes graphiques implémentent ensuite cette spécification à travers un driver. Ainsi, pour faire de l'OpenGL sur sa carte graphique, il suffit d'installer un driver récent depuis le site du constructeur ou depuis le gestionnaire de paquet.

Il est tout à fait possible d'implémenter l'ensemble des fonctions OpenGL dans une bibliothèque tournant sur CPU, c'est par exemple le cas de [MESA](http://www.mesa3d.org/).

## Contexte OpenGL

Le contexte OpenGL stocke l'ensemble des données OpenGL pour votre application (ensemble des variables d'état, tel que le status d'activation du depth test, et les objets créés).

La création du contexte OpenGL se fait le plus souvent à l'ouverture d'une fenêtre, par la bibliothèque bas niveau de gestion de fenêtre du systeme (X ou Wayland sous Linux, [la Win32 API sous Windows](https://www.khronos.org/opengl/wiki/Creating_an_OpenGL_Context_(WGL) ), etc.).

On n'a généralement pas à s'en occuper puisqu'il est courant de passer par une lib plus haur niveau de fenêtrage (SDL, GLFW, Qt, ...).

Plus d'informations sur le Wiki:

- https://www.khronos.org/opengl/wiki/OpenGL_Context
- https://www.khronos.org/opengl/wiki/Core_And_Compatibility_in_Contexts
- https://www.khronos.org/opengl/wiki/Get_Context_Info

## Calcul Asynchrone

Il est important de bien comprendre que les fonction de rendu (glDraw*) OpenGL sont généralement asynchrone. Cela signifie qu'elles ne sont pas executées immédiatement mais placées dans une command queue en attendant d'être executées par le GPU.

Cela permet au driver d'optimiser l'utilisation du GPU, et à l'application de pouvoir faire autre chose pendant que le GPU calcule.

La synchronisation entre CPU et GPU peut se faire implicitement via l'appel à certaines fonctions qui necessitent que le rendu soit terminé (glfwSwapBuffers), ou explicitement à l'aide des fonctions glFinish et glFlush.

- https://www.khronos.org/opengl/wiki/Synchronization

## Chargement des fonctions OpenGL Core Profile

OpenGL a un fonctionnement un peu particulier dans le sens ou il n'existe pas de header déclarant l'ensemble des fonctions OpenGL du core profile (i.e. OpenGL 3+).

Le header GL.h ne déclare qu'un nombre restreint de fonctions, principalement celles du compatibility profiles, dépréciées (glBegin, glEnd, glVertex3f, etc.).

Pour pouvoir appeler les fonctions OpenGL 3+, il faut demander à la lib de fenetrage bas niveau des pointeurs de fonctions sur chacune de celles que l'on veut utiliser.

Comme c'est une tache particulièrement ennuyante, répétitive et ininteressante, on passe généralement par une bibliothèque dediée qui va charger tous les pointeurs de fonction d'un coup et nous les exposer dans des variables globales.

La lib la plus utilisée est sans doute [GLEW](http://glew.sourceforge.net/), mais je vous recommande chaudement [GLAD](http://glad.dav1d.de/) qui à les avantages suivants:

- facile à compiler et linker, aussi bien sous windows que sous linux
- possibilité de customiser entièrement ce qu'elle contient via le site (choix de la version d'OpenGL, choix du profile, ajout d'extensions, etc.)