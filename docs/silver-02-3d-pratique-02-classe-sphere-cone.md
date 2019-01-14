---
id: silver-02-3d-pratique-02-classe-sphere-cone
title: Les classes Sphere et Cone
---

Ces classes sont fournient avec le template, dans la bibliothèque **glimac** partagée entre les TPs. Elles vous permettent de créer des tableaux de sommets contenant des triangles pour dessiner des spheres et des cones.

Par exemple:

```cpp
Sphere sphere(1, 32, 16);
```

Construit une sphere de rayon 1 et discrétisé selon 32 segments sur la lattitude et 16 sur la longitude. Un constructeur similaire existe pour Cone.

Ensuite la méthode **getVertexCount()** permet de récupérer le nombre de sommet et **getDataPointer()** renvoit un pointeur vers le tableau de sommets. Ces méthodes devront être utilisées pour envoyer les données à OpenGL (construction du VBO et du VAO).

Le type de sommet utilisé est définit dans **common.hpp**:

```cpp
struct ShapeVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};
```

Ce type devra être utilisé conjointement avec **sizeof** et **offsetof** pour construire le VAO (comme nous faisions avec les classes Vertex2DColor et Vertex2DUV).