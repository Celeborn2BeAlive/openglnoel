---
id: silver-05-post-processing-01-introduction
title: Introduction
---

L'objectif de ce dernier tutoriel est d'apprendre à appliquer des traitements à l'image après que celle ci ait été rendue. On appelle ça le post-processing.

Il y a plusieurs manière de faire du post-processing en OpenGL. La plus classique est d'utiliser un fragment shader qui prend en entrée une texture contenant l'image à traiter. Cela ressemble beaucoup à la shading pass du deferred rendering (la shading pass peut être vu comme un post-process du GBuffer).

Une manière plus efficace et plus moderne de faire du post-processing est d'utiliser un **compute shader**. Un **compute shader** permet de faire du GPGPU en OpenGL, c'est à dire du calcul parallele plus général que la rasterization. Un compute shader est très semblable à un kernel Cuda.

L'utilisation d'un compute shader permet d'éviter d'avoir a dessiner un quad pour lancer le traitement (car un compute shader est lancé sans géométrie d'entrée).

## Adaptation du code du deferred rendering

Nous allons repartir du deferred rendering afin de garder un code simple. Mais il va falloir l'adapter un peu: le dessin final ne doit plus être fait directement à l'écran, mais dans une texture. C'est très similaire au rendu du GBuffer: il faut utiliser un FBO.

- Dans la classe Application, ajoutez deux variable membres **GLuint m_BeautyTexture** et **GLuint m_BeautyFBO**.
- Dans le constructeur, initialisez cette texture et ce FBO. La texture doit avoir la taille de l'écran et être attachée au FBO. Son format interne doit être GL_RGBA32F. Le FBO doit avoir un seul draw buffer d'activé: GL_COLOR_ATTACHMENT0. Le code doit être similaire à l'initialisation du GBuffer.
- Au rendu, faite en sorte que la shading pass se dessine dans la texture. Il faut pour cela binder le FBO sur la cible **GL_DRAW_FRAMEBUFFER**.
- Utilisez glReadBuffer et glBlitFramebuffer pour afficher la texture de beauty à l'écran.

A ce stade vous devriez avoir un résultat identique au deferred classique.
