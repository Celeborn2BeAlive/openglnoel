---
id: gold-02-forward-renderer-04-transformations
title: Transformations
---

## Sans Caméra

Dans le code C++ de l'application, à l'initialisation:

- Charger et compiler les shaders (glmlv::compileProgram)
- Récupérer les locations des variables uniform
- Appeler .use() sur le programme GLSL compilé

Au rendu:

- Construire une matrice projection, une matrice view et deux matrices model (une pour le cube et une pour la sphere)
- Envoyez les combinaisons de ces matrices dans les uniform correspondante avant le rendu de chaque objet

A ce stade, vous devriez avoir un rendu correct de vos objets si vous vous arrangez pour bien construire les matrices (ne pas oublier qu'OpenGL "voit" du coté négatif de l'axe Z de l'espace view).

Fonctions glm à utiliser (header *glm/gtc/matrix_transform.hpp*):

- perspective
- translate
- rotate
- scale
- lookAt
- inverse et transpose (pour construire la normal matrix)

Fonctions OpenGL à utiliser:

A l'initialisation:

- glGetUniformLocation

Au rendu:

- glUniformMatrix4fv

Aide: [TD de l'année précédente](http://igm.univ-mlv.fr/~lnoel/index.php?section=teaching&teaching=opengl&teaching_section=tds&td=td6#intro)

## Avec Caméra

Utiliser la classe *ViewController* de *glmlv* afin de construire une view matrix qui se controle en mode FPS.