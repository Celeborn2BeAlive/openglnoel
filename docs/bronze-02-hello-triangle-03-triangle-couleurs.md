---
id: bronze-02-hello-triangle-03-triangle-couleurs
title: Dessiner un triangle en couleurs
---

L'objectif à présent est de dessiner un triangle en couleurs. Mais en OpenGL3, la couleur n'est pas gratuite: il faut utiliser des shaders. Nous verrons comment écrire et intéragir avec des shaders lors du prochain TP. Pour ce TP je vais vous fournir des shaders déja codés que vous chargerez en utilisant les classes fournies dans le template.

<span class="badge todo"></span> Dans le répertoire TP1, dupliquez le fichier exo1_triangle_blanc.cpp et renommez le exo2_triangle_couleurs.cpp. Faite l'exercice dans ce dernier.

<span class="badge todo"></span> Téléchargez les fichiers [triangle.vs.glsl](assets/triangle.vs.glsl) et [triangle.vs.glsl](assets/triangle.fs.glsl). Placez les dans le repertoire shaders du TP1.

<span class="badge todo"></span> Dans le fichier exo2_triangle_couleurs.cpp, ajoutez les include des fichier <glimac/Program.hpp> et <glimac/FilePath.hpp>. Dans le code de la fonction main, après l'initialisation de GLEW, ajoutez les lignes suivantes:

```cpp
FilePath applicationPath(argv[0]);
Program program = loadProgram(applicationPath.dirPath() + "shaders/triangle.vs.glsl",
                              applicationPath.dirPath() + "shaders/triangle.fs.glsl");
program.use();
```

Ce code à pour effet de charger les shaders, les compiler (à l'execution de l'application) et d'indiquer à OpenGL de les utiliser.

Il faut maintenant modifier le code pour ajouter une couleur à chaque sommet. Faisons le par étape.

Modifier le tableau de sommets
Les couleurs sont gérés en OpenGL par des nombres flottants entre 0 et 1 (et non pas 0 et 255 comme dans la plupart des logiciels de dessin). Nous allons associer la couleur rouge \\((1,0,0)\\) au premier sommet, vert \\((0,1,0)\\) au deuxième sommet et bleu \\((0,0,1)\\) au troisième sommet.

Pour cela on modifie le tableau de sommet en rajoutant la couleur après chaque position:

```cpp
GLfloat vertices[] = { -0.5f, -0.5f, 1.f, 0.f, 0.f, // premier sommet
0.5f, -0.5f, 0.f, 1.f, 0.f, // deuxième sommet
0.0f, 0.5f, 0.f, 0.f, 1.f // troisième sommet
 };
```

![VBO](/openglnoel/img/vbo.svg)

Il faut également modifier l'appel à glBufferData afin de mettre à jouer la taille du tableau qu'on lui envoit. Avant on lui passait 6 * sizeof(GLfloat), à présent c'est 15 * sizeof(GLfloat) (15 = le nombre de floats du tableau).

Activation et spécification des attributs de vertex
Puisqu'on rajoute l'attribut couleur, il faut modifier le spécifier. Il faut également modifier la spécification de l'attribut position car toute l'organisation du tableau de vertex à changé.

Connaitre l'index associé aux attributs
Comme mentionné précedemment, chaque attribut est identifié par un index. Dans l'exercice précédent, on n'utilisait pas de shader, et la position était identifié par l'index 0 (par défaut en OpenGL). A présent, on utilise des shaders. Pour connaitre l'index associé à chaque attribut, ouvrez le fichier triangle.vs.glsl que vous avez téléchargez. En début de fichier, vous devriez lire les lignes:

```cpp
layout(location = 3) in vec2 aVertexPosition;
layout(location = 8) in vec3 aVertexColor;
```

Ces deux lignes déclarent les attributs de vertex du shader. la partie location = X indique l'index associé à chaque attribut. Notez bien que c'est moi qui ait choisit les index 3 et 8 dans ce cas. J'aurais très bien pu choisir d'autres nombres !

<span class="badge todo"></span> En utilisant les index déclarés dans le fichier, modifiez votre code C++ de manière à activer les deux attributs de vertex position et couleur (rappel: avec la fonction glEnableVertexAttribArray). Vous devez normalement avoir deux appels à cette fonctions.

Spécifier les attributs de vertex
Il faut à présent modifier l'appel à glVertexAttribPointer pour l'attribut position et en rajouter un pour l'attribut couleur.

Voici un rappel de la signature de la fonction et des paramètres:

```cpp
glVertexAttribPointer(GLuint index, 
    GLint size, GLenum type, GLboolean normalized, 
    GLsizei stride, const GLvoid* pointer);
```

Paramètres:

- GLuint index: l'index de l'attribut à spécifier
- GLint size: le nombre de composantes de l'attribut.
- GLenum type: une constante OpenGL indiquant le type de chaque composante. Dans notre cas on passe GL_FLOAT pour indiquer que chaque composante est un nombre flottant.
- GLboolean normalized: pas besoin de s'occuper de ce paramètre pour le moment, on passe GL_FALSE
- GLsizei stride: indique à OpenGL le nombre d'octets séparant l'attribut pour deux sommets consécutifs.
- const GLvoid* pointer: un pointeur, ou pas. Ce paramètre est un piège. Il faut en fait passer l'offset (décalage) en octets de la premiere instance de l'attribut dans le tableau.

Ce schéma vous indique les paramètres à passer pour chaque attribut et comment les trouver:

![VAO](/openglnoel/img/vao.svg)

<span class="badge todo"></span> Modifier votre premier appel à glVertexAttribPointer pour spécifier l'attribut position correctement. Ajouter juste après un deuxième appel afin de spécifier l'attribut couleur. A noter qu'il faut caster le dernier paramètre (l'offset) en (const GLvoid*) afin d'éviter une erreur de compilation.

## Tester le code

C'est terminé ! et oui c'était rapide par rapport à l'exercice précédent. Vous pouvez compiler et tester le code (n'oubliez pas de relancer CMake car le script se charge de copier-coller les shaders à coté de l'éxecutable compilé). Le résultat attendu est:

![Triangle couleurs](/openglnoel/img/triangle.png)