---
id: gold-02-forward-renderer-02-geometrie
title: Geometrie
---

> Vous aurez un écran noir jusqu'a la fin de la première partie de l'exercice [Transformations](/02-forward-shading/transformations/): OpenGL demande généralement beaucoup de code d'initialisation avant de pouvoir afficher quelque chose.

Vous allez commencer par dessiner des cubes et spheres en 3D. La lib *glmlv* contient deux fonctions makeCube() et makeSphere() (simple_geometry.hpp) permettant de construire des tableaux de sommets et indices pour ces deux forme simple.

A l'initialisation (constructeur de Application):

- Utiliser ces fonctions pour construire la géométrie d'une sphere et d'un cube.
- Pour chacune de ces formes, construire un VBO, un VAO et un IBO et les remplir correctement.
- Activer le depth test

Dans la boucle de rendu (Application::run):

- Adapter l'appel à glClear pour aussi nettoyer le depth buffer
- Pour chaque objet: binder le VAO, dessiner l'objet

Fonctions OpenGL à utiliser à l'initialisation:

| Sans DSA                             | DSA |
| ------------------------------------ | ----------- |
| glGenBuffers                         | glCreateBuffers |
| glGenVertexArrays                    | glCreateVertexArrays |
| glBindBuffer(GL_ARRAY_BUFFER, vboID) |
| glBufferStorage                      | glNamedBufferStorage |
|                                      | glVertexArrayVertexBuffer |
|                                      | glVertexArrayAttribBinding |
| glEnableVertexAttribArray            | glEnableVertexArrayAttrib |
| glVertexAttribPointer                | glVertexArrayAttribFormat |
| glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID) | glVertexArrayElementBuffer |
| glEnable(GL_DEPTH_TEST)              | glEnable(GL_DEPTH_TEST) |

Fonctions OpenGL à utiliser au rendu:

- glBindVertexArray
- glDrawElements (le nombre de sommets à dessiner est le nombre d'elements dans l'indexBuffer)
- glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

Voir le programme *quad* du template pour un exemple simple en 2D.

> A l'initialisation du VAO, il faut utiliser pour attribut de vertex (position, normal, texCoords) un entier correspondant à la location de cet attribut dans le vertex shader.
Le VS n'est pas encore écrit mais on peut déjà choisir les location (le plus simple: position en 0, normal en 1 et texCoords en 2).