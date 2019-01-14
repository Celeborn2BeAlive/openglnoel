---
id: gold-06-course-09-shaders
title: Shaders
---

Les shaders sont de cours programmes compilés pour fonctionner sur carte graphique, dans les stages dit programmables du pipeline.

Plusieurs languages de shading existent mais nous utilisons GLSL dans ces TPs car OpenGL fournie une API et compilation et link pour ce language.

A lire: https://www.khronos.org/opengl/wiki/OpenGL_Shading_Language

Différents types de shaders existent, ceux que nous utilisons dans ces TPs sont:

- [Vertex Shaders](https://www.khronos.org/opengl/wiki/Vertex_Shader): Se charge du calcul par sommet. Généralement il projete le sommet en entrée sur l'écran afin que les triangles soit prêt pour la rasterisation.
- [Fragment Shaders](https://www.khronos.org/opengl/wiki/Fragment_Shader): Se charge du calcul par fragment. Les fragments sont générés par la rasterisation à partir des triangles, en interpolant les attributs aux sommets. Lorsqu'un fragment n'est pas occulté par un autre, il apparait dans un pixel de l'image finale, dont la couleur a été calculée par le fragment shader.
- [Geometry Shaders](https://www.khronos.org/opengl/wiki/Geometry_Shader): Shader optionnel qui vient se placer après le vertex shader et avant la rasterisation. Il implémente un calcul par primitive (point, ligne ou triangle) et à la possibilité de générer de nouvelles primitives à partir de sa primitive d'entrée.
- [Compute Shaders](https://www.khronos.org/opengl/wiki/Compute_Shader): Petit dernier introduit dans la spécification, le compute shader permet d'implémenter un calcul parallèle quelquonque sur GPU, en dehors du pipeline. Il permet de faire du GPGPU en OpenGL, comme en Cuda ou en OpenCL.
