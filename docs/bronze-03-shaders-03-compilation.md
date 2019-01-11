---
id: bronze-03-shaders-03-compilation
title: Charger, compiler et utiliser
---

Pour utiliser un couple de shaders il faut fournir leur code source à OpenGL puis lui demander de compiler les shaders pour produire un programme GPU. Ce programme peut ensuite être utilisé avant de dessiner. Il est possible de changer de programme à la volée pour dessiner des objets différent avec des shaders différent. Cette section vous présente rapidement les fonctions permettant de créer et compiler des shaders en OpenGL sans rentrer dans les détails. Les fonctions de la bibliothèque glimac fournies dans le template de code vous permettent de faire tout ça très rapidement.

## Création et compilation d'un shader

OpenGL fournit les fonctions suivantes:

- [glCreateShader](http://docs.gl/gl4/glCreateShader) qui permet de créer un nouveau shader et renvoit son identifiant OpenGL.
- [glShaderSource](http://docs.gl/gl4/glShaderSource) qui permet de spécifier le code source du shader sous la forme de chaines de caractères. Lorsque le shader est stocké dans un fichier il est necessaire de le charger préalablement (ce que vous pourrez faire avec le code fournit).
- [glCompileShader](http://docs.gl/gl4/glCompileShader) qui permet de compiler un shader. Les shaders sont donc compilés au moment de l'execution de votre programme, lors de l'initialisation le plus souvent.
- [glGetShaderInfoLog](http://docs.gl/gl4/glGetShaderInfoLog) qui nous permet d'obtenir des messages d'erreur / warning relatifs à la compilation d'un shader.
La pipeline de compilation d'un shader peut donc être représentée par ce schéma:

![Shader compilation](/openglnoel/img/shader_compilation.svg)

## Création et link d'un programme

Une fois qu'un VS et un FS ont été compilés, il faut les lier en un programme. Pour cela OpenGL fournit les fonctions suivantes:

- [glCreateProgram](http://docs.gl/gl4/glCreateProgram) qui permet de créer un nouveau programme GLSL et renvoit son identifiant OpenGL.
- [glAttachShader](http://docs.gl/gl4/glAttachShader) qui permet d'attacher un shader à un programme. Il faut attacher le VS et le FS au programme.
- [glLinkProgram](http://docs.gl/gl4/glLinkProgram) qui permet de linker les deux shaders et ainsi avoir un programme utilisable.
- [glGetProgramInfoLog](http://docs.gl/gl4/glGetProgramInfoLog) qui nous permet d'obtenir des messages d'erreur / warning relatifs au link du programme.

![Program link](/openglnoel/img/shader_link.svg)

## Utiliser le code fourni

Le code que je vous ai fournit vous permet de charger, compiler et utiliser des shaders GLSL. Vous l'avez déjà utilisé au TP1 pour afficher le triangle en couleur.

Pour que le tout fonctionne bien, les shaders doivent être placés dans le repertoire **shader** correspondant au TP courant (donc **TP2** actuellement). Le fichier CMake se charge de copier ce repertoire dans le build afin d'avoir un chemin de fichier relatif à l'executable. Ainsi, à chaque fois que vous modifiez vos shaders, il faut relancer cmake (et pas juste make), sinon les modifications ne seront pas prise en compte. C'est un peu fastidieux mais plus simple à gérer ensuite dans le code.

La classe **glimac::Program** représente un programme GPU. La fonction **glimac::loadProgram** charge et compile un vertex shader et un fragment shader pour produire un programme. Si une erreur de compilation est detectée, une exception est lancée par la fonction et arrête votre executable. L'erreur est affichée et vous pouvez la corriger.

Le code type pour charger des shaders est donc:

```cpp
FilePath applicationPath(argv[0]);
Program program = loadProgram(applicationPath.dirPath() + "shaders/[VERTEX_SHADER]",
	applicationPath.dirPath() + "shaders/[FRAGMENT_SHADER]");
```

Il faut bien évidemment remplacer [VERTEX_SHADER] par le nom du fichier contenant le code source du VS et [FRAGMENT_SHADER] par le nom du fichier contenant le code source du FS.

Concernant la variable applicationPath, elle contient le chemin de votre executable. **applicationPath.dirPath()** permet de récuperer le chemin du repertoire contenant l'executable. Par conséquent, **applicationPath.dirPath() + "shaders/[VERTEX_SHADER]"** sera le chemin vers le vertex shader copié par CMake. De même pour le fragment shader.

<span class="badge todo"></span> Dans le TP2, copiez le code C++ permettant de dessiner un triangle en couleur (celui du TP1).

<span class="badge todo"></span> Dans le dossier shader du TP2, créez deux fichiers color2D.vs.glsl et color2D.fs.glsl. Dans ces deux fichiers, copiez-coller le code des shaders expliqués précédement:

Vertex Shader:

```cpp
#version 330 core

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec3 aVertexColor;

out vec3 vFragColor;

void main() {
  vFragColor = aVertexColor;
  gl_Position = vec4(aVertexPosition, 0, 1);
};
```

Fragment Shader:

```cpp
#version 330 core

in vec3 vFragColor;

out vec3 fFragColor;

void main() {
  fFragColor = vFragColor;
};
```

<span class="badge todo"></span> Modifiez le code C++ copié afin qu'il fonctionne avec ces shaders. Il y a deux choses à changer:

- Le chargement des shaders: il faut changer le nom des fichiers
- L'index des attributs: ce n'est plus 3 et 8 mais 0 et 1.

<span class="badge todo"></span> Dans le build, lancez cmake et make puis executez le programme. Le triangle en couleur devrait s'afficher correctement. Afin de voir comment se comporte le programme lorsqu'un shader contient une erreur, introduisez en une dans le Vertex Shader, relancez CMake, puis votre programme. Constatez l'erreur puis corrigez la. (Une erreur simple à introduire: rajoutez "bhbfhzebfhbzefjze" juste avant la fonction main du shader).