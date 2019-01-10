---
id: bronze-02-hello-triangle-04-vertex-structure
title: Utiliser une structure de vertex
---

Jusqu'a présent nous avons envoyé nos sommets comme des sauvages sous la forme de flottants. Nous allons maintenant utiliser une structure pour représenter un vertex, ce qui permettra d'avoir un code plus clair à lire. Dupliquez le fichier exo2_triangle_couleur.cpp et renommez le exo3_structure_vertex.cpp puis codez dedans.

## Définir la structure

<span class="badge todo"></span>  Ajoutez l'include du fichier glimac/glm.hpp. Ce fichier permet d'utiliser la bibliothèque de maths GLM qui nous permettra d'avoir des types vecteur, matrices et toutes les opérations associées.

<span class="badge todo"></span>  Avant la fonction main, définir la structure Vertex2DColor possédant deux champs glm::vec2 position et glm::vec3 color. Lui définir également un constructeur vide et un constructeur prenant en paramètre la position et la couleur pour remplir les champs de la structure (c'est la partie ou j'espère que vous avez déjà vu ça en C++ pour ne pas avoir à l'expliquer en TP !)

## Modifier le tableau de sommet

<span class="badge todo"></span>  Modifiez le tableau de vertex pour utiliser votre structure à la place des floats:

```cpp
GLfloat vertices[] = { -0.5f, -0.5f, 1.f, 0.f, 0.f, // premier sommet
    0.5f, -0.5f, 0.f, 1.f, 0.f, // deuxième sommet
    0.0f, 0.5f, 0.f, 0.f, 1.f // troisième sommet
```

devient:

```cpp
Vertex2DColor vertices[] = { 
    Vertex2DColor(glm::vec2(-0.5, -0.5), glm::vec3(1, 0, 0)),
    Vertex2DColor(glm::vec2(0.5, -0.5), glm::vec3(0, 1, 0)),
    Vertex2DColor(glm::vec2(0, 0.5), glm::vec3(0, 0, 1))
}
```

<span class="badge todo"></span>  Modifiez l'appel à glBufferData afin d'utiliser 3 * sizeof(Vertex2DColor) à la place de 15 * sizeof(GLfloat). Il se trouve que c'est la même chose, mais 3 * sizeof(Vertex2DColor) c'est plus clair, on comprend mieux qu'on à 3 sommets.

## Clarifier la spécification des sommets

Il est possible d'améliorer les appels à glVertexAttribPointer pour utiliser notre structure. En effet, pour chacun des deux appels le paramètre stride est 5 * sizeof(GLfloat), ce qui correspond à sizeof(Vertex2DColor) (car notre structure est composé d'un vec2 et d'un vec3, soit 2 + 3 = 5 floats).

De plus le paramètre pointer correspond à l'offset de chaque attribut par rapport au début du tableau. Cela correspond également à l'offset de chaque champs dans la structure. La macro offsetof de la bibliothèque standard C++ permet de calculer cette offset automatiquement en spécifiant le nom de la structure et le champs.

Ainsi, offsetof(Vertex2DColor, position) vaut 0, soit l'offset de l'attribut position. offsetof(Vertex2DColor, color) vaut 2 * sizeof(GLfloat), soit l'offset de l'attribut color.

<span class="badge todo"></span>  Modifiez vos deux appels à glVertexAttribPointer comme indiquez ci dessus. Il se peut que vous ayez à inclure le header cstddef pour utiliser la macro offsetof.