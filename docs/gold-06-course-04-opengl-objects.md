---
id: gold-06-course-04-opengl-objects
title: Objets OpenGL
---

Les objets OpenGL représentent des conteneurs d'état du contexte OpenGL. 
Lorsqu'un objet est bindé sur l'état correspondant, tous les changement sur cet état sont stocké dans l'objet bindé.

A lire: https://www.khronos.org/opengl/wiki/OpenGL_Object

Il existe différents types d'objets OpenGL, ceux que nous utilisons dans ces TPs étant:

- [Buffer Object](https://www.khronos.org/opengl/wiki/Buffer_Object): représente un tableau de données pouvant être stocké en mémoire GPU.
- [Vertex Array Object](https://www.khronos.org/opengl/wiki/Vertex_Specification): représente la spécification des sommets et leurs rangement aux seins d'un ou plusieurs buffer objets.
- [Texture Object](https://www.khronos.org/opengl/wiki/Texture): représente une texture pouvant être stocké en mémoire GPU et pouvant être lue et filtrée depuis les shaders.
- [Sampler Object](https://www.khronos.org/opengl/wiki/Sampler_Object): représente des paramètres de lecture et filtrage de textures.
- [Framebuffer Object](https://www.khronos.org/opengl/wiki/Framebuffer_Object): Représente un ensemble de textures sur lesquels il est possible de dessiner.
- [GLSL Objects](https://www.khronos.org/opengl/wiki/GLSL_Object): Shaders et programmes GLSL qui peuvent être executés par les stage programmable du GPU.
