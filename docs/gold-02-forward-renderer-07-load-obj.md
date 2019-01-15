---
id: gold-02-forward-renderer-07-load-obj
title: Chargement de modèles OBJ
---

> Vous pouvez obtenir différents modèles OBJ sur [cette page](https://casual-effects.com/data/) (je vous conseille de commencer par essayer avec Crytek-Sponza).

## API

Utiliser la fonction suivante de la lib pour charger un modèle OBJ:

```cpp
void glmlv::loadAssimpScene(const glmlv::fs::path & objPath, glmlv::SceneData & data);
```

Celle ci charge le fichier dont le chemin est passé en paramètre (pensez à mettre le modèle dans le repertoire *assets*).
Elle remplit une structure de type **glmlv::SceneData** contenant les informations suivantes:

```cpp
struct SceneData
{
    // Points min et max de la bounding box englobant la scene
    glm::vec3 bboxMin = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 bboxMax = glm::vec3(std::numeric_limits<float>::lowest());

    std::vector<Vertex3f3f2f> vertexBuffer; // Tableau de sommets
    std::vector<uint32_t> indexBuffer; // Tableau d'index de sommets

    size_t shapeCount = 0; // Nombre d'objets à dessiner
    std::vector<uint32_t> indexCountPerShape; // Nomber d'index de sommets pour chaque objet
    std::vector<glm::mat4> localToWorldMatrixPerShape; // Matrice localToWorld de chaque objet
    std::vector<int32_t> materialIDPerShape; // Index du materiau de chaque objet (-1 si pas de materiaux)

    std::vector<PhongMaterial> materials; // Tableau des materiaux
    std::vector<Image2DRGBA> textures; // Tableau des textures référencés par les materiaux
}
```

Les tableaux vertexBuffer et indexBuffer doivent être utilisés pour remplir un VBO et un IBO à binder sur un VAO pour le rendu.

Les champs bboxMin et bboxMax permettent de connaitre les dimensions de la scene afin d'adapter la vitesse de la caméra et la matrice de projection. Par exemple:

```cpp
SceneData data;
loadAssimpScene(path, data);

const auto sceneDiagonalSize = glm::length(data.bboxMax - data.bboxMin);
m_viewController.setSpeed(sceneDiagonalSize * 0.1f); // 10% de la scene parcouru par seconde

const auto projMatrix = glm::perspective(70.f, float(viewportSize.x) / viewportSize.y, 0.01f * m_SceneSize, m_SceneSize); // near = 1% de la taille de la scene, far = 100%
```

Le tableau indexCountPerShape doit être utilisé lors du rendu pour dessiner chacun des objets:

```cpp
glBindVertexArray(m_SceneVAO);
auto indexOffset = 0;
for (const auto indexCount: indexCountPerShape)
{
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (const GLvoid*) (indexOffset * sizeof(GLuint)));
    indexOffset += indexCount
}
```

Dans le code ci-dessus, l'index offset permet de dire à OpenGL ou se trouve les indices du i-ème objet dans le tableau d'indices.

Le tableau textures contient les textures référencées dans l'OBJ. Après chargement de l'OBJ, il faut créer une texture OpenGL pour chacune.

Le tableau materials contient l'ensemble des materiaux référencés dans la scène. La fonction loadAssimpScene ne charge que les données de matériaux nécessaires à un modèle de shading de Blinn-Phong (Ambiant + Diffus + Glossy).

Finalement, le tableau materialIDPerShape indique le materiau de chaque objet (l'index d'un élement du tableau materials).

A noter que si un objet n'a pas de matériau, -1 sera placé à sa position dans materialIDPerShape.
De la même manière, si un champs texture n'est pas renseigné pour un matériaux, alors il est mis à -1.

Il faut donc, après avoir appelé cette fonction, initialiser toutes les données GPU (un VBO, un IBO, un VAO, des textures object, un texture sampler, les uniform location, le programme GLSL) afin de pouvoir dessiner la scène dans la boucle de rendu.

Comme pour les exercices précédent, il est conseillé d'y aller par étape. D'abord afficher uniquement les normales des objets 3D, puis afficher la couleur diffuse, puis faire l'illumination.

## Modèle de Shading

Améliorer le modèle de shading utilisé pour utiliser le modèle de Blinn-Phong plutot qu'un simple modèle Diffus (voir [cet ancien TD](gold-02-forward-renderer-07-load-obj) pour les équations).

Cela implique de rajouter des variables uniformes pour le coefficient speculaire, la texture speculaire, l'exposant de shininess et la texture de shininess.

Vous pouvez également rajouter le terme ambiant en addition.

Toutes les informations necessaires sont chargés dans les matériaux de SceneData.

> Si vous bloquez sur un element du TP, vous pouvez puller la branche *cheat* du repo et regarder le code de l'application *forward-renderer-06-load-obj*.