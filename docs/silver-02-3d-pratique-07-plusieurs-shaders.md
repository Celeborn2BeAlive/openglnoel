---
id: silver-02-3d-pratique-07-plusieurs-shaders
title: Utiliser plusieurs shaders
---

Là ça devient compliqué car chaque programme GPU (un programme = un vertex shader + un fragment shader) possède son propre ensemble de variables uniformes. Par exemple, si deux programmes utilise la variable uniforme "uMVPMatrix", celle ci peu avoir une location différente pour chacun des deux programme. Il faudra donc récupérer deux fois sa location: une fois pour le premier programme et une fois pour le second programme.

Afin d'avoir un code à peu près propre, nous allons utiliser une structure pour représenter chaque programme. La structure **EarthProgram** contiendra le programme pour dessiner la terre, ainsi que les locations de chacun de ses variables uniforme. La structure **MoonProgram** contiendra la même chose mais pour les shaders correspondant à la lune.

Voici un exemple de ces deux structure dans mon code:

```cpp
struct EarthProgram {
    Program m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint uEarthTexture;
    GLint uCloudTexture;

    EarthProgram(const FilePath& applicationPath):
        m_Program(loadProgram(applicationPath.dirPath() + "shaders/3D.vs.glsl",
                              applicationPath.dirPath() + "shaders/multiTex3D.fs.glsl")) {
        uMVPMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        uMVMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        uEarthTexture = glGetUniformLocation(m_Program.getGLId(), "uEarthTexture");
        uCloudTexture = glGetUniformLocation(m_Program.getGLId(), "uCloudTexture");
    }
};

struct MoonProgram {
    Program m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint uTexture;

    MoonProgram(const FilePath& applicationPath):
        m_Program(loadProgram(applicationPath.dirPath() + "shaders/3D.vs.glsl",
                              applicationPath.dirPath() + "shaders/tex3D.fs.glsl")) {
        uMVPMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        uMVMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        uTexture = glGetUniformLocation(m_Program.getGLId(), "uTexture");
    }
};
```

Puisque le shader **tex3D.fs.glsl** utilise une seule variable uniforme de texture, il n'y a qu'une seule variable membre pour stocker la location dans la structure MoonProgram ( contrairement à la structure EarthProgram qui en contient deux).

Il faut voir chacune de ces structure comme la représentation CPU du programme GPU associé. Elles nous permettent de faire facilement l'interface entre CPU et GPU.

<span class="badge todo"></span> Ajoutez ces deux structures à votre code. Si besoin modifiez le nom des variables uniformes dans le constructeur ainsi que le nom de la variable membre associée pour stocker la location.

<span class="badge todo"></span> Dans la fonction main du code C++, remplacez le chargement des shaders par une déclaration d'une variable pour chacune des structures:

```cpp
FilePath applicationPath(argv[0]);
EarthProgram earthProgram(applicationPath);
MoonProgram moonProgram(applicationPath);
```

Puisque nous avons à présent deux programmes GPU, on ne peut plus faire un ".use()" global dans la partie initialisation comme on le faisait jusqu'a présent. Il va falloir, avant le dessin de chaque entitée (terre ou lune), changer le programme utilisé en utilisant la méthode use() sur la variable membre **m_Program** de la structure adaptée. De même pour modifier les variables uniformes. Voici par exemple mon code pour dessiner la terre, dans la boucle de rendu:

```cpp
earthProgram.m_Program.use();

glUniform1i(earthProgram.uEarthTexture, 0);
glUniform1i(earthProgram.uCloudTexture, 1);

glm::mat4 globalMVMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, -5));

glm::mat4 earthMVMatrix = glm::rotate(globalMVMatrix, windowManager.getTime(), glm::vec3(0, 1, 0));
glUniformMatrix4fv(earthProgram.uMVMatrix, 1, GL_FALSE, 
	glm::value_ptr(earthMVMatrix));
glUniformMatrix4fv(earthProgram.uNormalMatrix, 1, GL_FALSE, 
	glm::value_ptr(glm::transpose(glm::inverse(earthMVMatrix))));
glUniformMatrix4fv(earthProgram.uMVPMatrix, 1, GL_FALSE, 
	glm::value_ptr(projMatrix * earthMVMatrix));

glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, earthTexture);
glActiveTexture(GL_TEXTURE1);
glBindTexture(GL_TEXTURE_2D, cloudTexture);

glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());
```

On observe que j'utilise la variable earthProgram pour activer le bon programme et modifier les bonnes uniformes. Pour dessiner la lune il faut utiliser la variable moonProgram.

<span class="badge todo"></span> Modifiez votre code pour utiliser vos deux programmes séquentiellement.

Concernant l'appel à la méthode **use()**: celle ci est assez couteuse, on ne peut pas se permettre de changer de programme GPU à chaque dessin. C'est pourquoi avant cet exercice on le faisait en dehors de la boucle de rendu. Puisque vous dessinez plusieurs lunes, vous pourriez être tenté de faire l'appel **moonProgram.m_Program.use()** à chaque tour de la boucle qui dessine les lunes. La bonne solution est de faire cet appel avant cette boucle. Ainsi on ne fera pas:

```cpp
for(uint32_t i = 0; i < nbMoon; ++i) {
	moonProgram.m_Program.use();

	// Modification des uniformes ...

	glDrawArrays(...);
}
```

Mais plutot:

```cpp
moonProgram.m_Program.use();
for(uint32_t i = 0; i < nbMoon; ++i) {
	// Modification des uniformes ...

	glDrawArrays(...);
}
```

Dans un vrai moteur, on regroupe les appels de dessin de manière à minimiser le nombre de changement de shaders. Si on dessiner 42 terres et 69 lunes, on dessinerais d'abord toutes les terres avec le programme GPU pour dessiner la terre, puis ensuite toutes les lunes avec le programme GPU pour dessiner la lune. Cela permet de changer seulement deux fois de programme par tour de boucle de rendu.

Si au contraire on alternait dessin terre - dessin lune - dessin terre - dessin lune - etc. on devrait à chaque fois changer de programme, ce qui tuerait les performances (vous pouvez essayer si le coeur vous en dit !).