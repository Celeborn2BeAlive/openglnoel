---
id: bronze-03-shaders-02-exemple
title: Les shaders par l'exemple
---

Rien de vaut un exemple. Nous allons étudier ligne par ligne l'exemple des shaders que vous avez utilisé au TP précédent pour afficher un triangle en couleur (avec quelques variantes).

## Le Vertex Shader

Le Vertex Shader est destiné à traiter des sommets. Il fera donc son traitement à partir des attributs de chaque vertex. Il fournit en sortie des valeurs à interpoler pour chaque fragment produit par le rasterizer sur le triangle. Il renseigne également une variable particulière, gl_Position, qui est la position projetée à l'écran. Celle ci est utilisée par le rasterizer: il combine les 3 gl_Position correspondant à un triangle afin de produire une multitude de fragments. OpenGL se charge d'executer le Vertex Shader branché au moment du draw call sur chaque sommet à dessiner.

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

![Vertex shader par l'example](/openglnoel/img/vertex_shader_example.svg)

Comme vous pouvez le constater, cela ressemble à du C++. Il y a même une fonction **main**, qui sera executée par le GPU pour traiter chaque sommet à dessiner.

La première ligne indique à OpenGL la version de GLSL à utiliser. Ici on demande explicitement la version 3.3 avec un profile Core (qui bannit les fonctionnalités dépreciées).

Les deux lignes suivantes:

```cpp
layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec3 aVertexColor;
```

indiquent les attributs d'entrée constituant un sommet ainsi que leur type et leur **location** La location permet de faire le lien avec l'application C++: lorsque vous configurez le VAO décrivant vos sommet, il faut utiliser pour chaque attribut activé la location correspondant à l'attribut dans le shader (Il existe une alternative pour spécifier la location, en utilisant la fonction **glBindAttribLocation** depuis l'application C++, mais nous ne l'utiliserons pas). Le mot clef **in** indique que c'est une **variable d'entrée** qui est déclarée.

La location correspond au paramètre index des fonctions **glEnableVertexAttribArray** et **glVertexAttribPointer**. Voici un rappel du code C++ pour faire le lien entre les deux concepts:

```cpp
const GLuint VERTEX_ATTR_POSITION = 0;
const GLuint VERTEX_ATTR_COLOR = 1;
glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
glEnableVertexAttribArray(VERTEX_ATTR_COLOR);
glBindBuffer(GL_ARRAY_BUFFER, vbo);
glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), 
	(const GLvoid*) offsetof(Vertex2DColor, position));
glVertexAttribPointer(VERTEX_ATTR_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), 
	(const GLvoid*) offsetof(Vertex2DColor, color));
glBindBuffer(GL_ARRAY_BUFFER, 0);
```

OpenGL "comprend" comment appliquer le vertex shader à chaque sommet en utilisant les locations que vous lui fournissez. C'est à vous de choisir les location/index associé à chaque attribut. Ici j'ai choisi 0 et 1 mais dans le TP précédent j'avais choisi 3 et 8 (pour vous embrouiller mwahaha).

La ligne:

```cpp
out vec3 vFragColor;
```

déclare une variable de sortie du shader. Je l'ai appelé **vFragColor** car elle correspond à la couleur qui sera affectée à chaque fragment (j'aurais pu l'appeler "toto" mais ça aurait été moins clair). Nous verrons plus loin que cette variable arrivera en entrée du fragment shader.

On pourrait également déclarer d'autres variables de sortie, ce que nous ferons plus tard en fonction de l'algorithme de rendu que l'on souhaite implanter.

Viens ensuite la fonction **main**. Contrairement au C++, celle ci ne renvoit pas de valeur en GLSL.

La ligne:

```cpp
vFragColor = aVertexColor;
```

affecte la couleur d'entrée à la variable de sortie. On pourrait effectuer un traitement plus complexe, comme faire varier la couleur en fonction de la position. Ici on fait simplement un transfert de la valeur d'entrée vers la valeur de sortie (cela arrivera souvent dans les Vertex Shaders).

Enfin la ligne:

```cpp
gl_Position = vec4(aVertexPosition, 0, 1);
```

est sans doute la plus importante du shader. On renseigne la variable built-in (built-in = définie par OpenGL) **gl_Position** qui est utilisée par OpenGL dans le rasterizer pour transformer les triangles en fragments. Cette variable représente les coordonnées projetées à l'écran du sommet. Dans le cas de la 2D il n'y a pas de projection, aucune transformation n'est donc appliquée ici. On ajoute néammoins deux coordonnées. Une profondeur z = 0 car on considère qu'on regarde le plan 2D z = 0 (on pourrait mettre n'importe quelle valeur entre -1 et 1, cela ne changerait rien). On ajoute également une coordonnée homogène (OpenGL travaille avec des coordonnées homogènes) w = 1. Quand nous ferons de la 3D, cette coordonnée sera calculée par la transformation projective. Pour l'instant, en **2D**, gardez à l'esprit qu'on ajoutera toujours 0 et 1 à la fin de nos positions.

## Le Fragment Shader

Le Fragment Shader est destiné à traiter des fragments (ie. des pixels). Il fera son traitement à partir des valeurs aux sommets interpolées par la rasterizer lors de la production des fragments. Il fournit en sortie une couleur pour le pixel associé au fragment (OpenGL gère l'association fragment-pixel, il n'y a donc pas à spécifier les coordonnées du pixel).

```cpp
#version 330 core

in vec3 vFragColor;

out vec3 fFragColor;

void main() {
  fFragColor = vFragColor;
};
```

![Fragment shader par l'example](/openglnoel/img/fragment_shader_example.svg)

La ligne:

```cpp
in vec3 vFragColor;
```

indique à OpenGL une valeur d'entrée du fragment shader. Deux choses à noter:

- elle à le même nom que la valeur de sortie du VS. C'est normal, et même obligatoire. OpenGL utilise les noms des variables pour savoir comment faire correspondre les valeurs de sortie du VS avec les valeurs d'entrée du FS. C'est un peu comme brancher une prise, l'entrée et la sortie doivent correspondre. A noter que le type est le même également.
- Contrairement aux variables d'entrées du VS, il n'y a pas de déclaration de la forme layout(location = ...). C'est inutile en effet car la location est spécifiée pour faire le lien entre l'application C++ et le VS. Or les données d'entrée du FS ne viennent pas de l'application ! elles viennent du rasterizer.

<span class="badge warning"></span> Les valeurs que récuperera notre fragment shaders ne seront pas exactement les mêmes que celles sortant du VS: il ne faut pas oublier que ces dernières sont interpolées sur toute la surface du triangle afin d'être affecté à chaque fragment.

La ligne:

```cpp
out vec3 fFragColor;
```

déclare la variable de sortie du FS destinée à contenir la couleur finale du pixel correspondant au fragment. Vous pouvez nommer cette variable comme vous voulez: OpenGL sait que s'il n'y a qu'une seule variable de sortie alors il doit l'interpreter comme une couleur et l'afficher à l'écran si le fragment est visible.

A noter qu'il est possible d'avoir plusieurs variables de sortie (pour écrire dans plusieurs images à la fois) mais nous n'utiliserons pas cette fonctionnalité durant ces TPs.

Et enfin le main ne fait qu'une chose: transférer la couleur d'entrée vers la couleur de sortie. A nouveau on pourrait appliquer des traitements sur cette couleur pour faire quelque chose de plus interessant.

## Des vrais shaders

Ces shaders sont vraiment simples et ne font pas grand chose (c'est toujours mieux que du blanc mais bon...). Le site [Shadertoy](https://www.shadertoy.com/) permet de coder en ligne (WebGL powa !) des shaders afin de les mettre à disposition d'autres utilisateurs. Vous avez donc accès à des centaines d'exemples de shaders auquels vous ne comprendrez pas grand chose (pour l'instant !). Il vous suffit de passer la souris sur une des vignette pour que l'animation se déclenche en temps réel. Les PC de l'université ont un peu du mal (c'est vraiment pas des shaders de tapette :p) mais vous devriez pouvoir constater la puissance à laquelle vous avez accès avec les shaders.

Il est interessant de constater que la plupart des shaders de ce site sont entièrement procéduraux, c'est à dire qu'il n'y a aucune modélisation 3D derrière: tout est calculé mathématiquement à la volée (vous voyez que les maths c'est de l'art :p).