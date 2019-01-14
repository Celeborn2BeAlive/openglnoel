---
id: silver-02-forward-renderer-06-textures
title: Textures
---

## Fragment Shader

- Ajouter une variable uniform *sampler2D uKdSampler*, destiné à pointer sur une texture des couleurs diffuses pour l'objet en cours de rendu.
- Dans le main, utiliser la fonction GLSL [*texture*](http://docs.gl/sl4/texture) afin de lire le sampler en utiliser les tex coords du fragment.
- Multiplier la valeur lue avec la variable uKd pour obtenir le coefficient diffus final de l'objet, à utiliser pour l'illumination

## Application

A l'initialisation

- Charger deux images de votre choix à utiliser en temps que texture diffuse de chacun de nos objets (utiliser la classe glmlv::Image2DRGBA et la fonction glmlv::readImage)
- Construire deux texture objects OpenGL et envoyer les pixel des deux images chargés dans ces texture objects
- Construire un sampler object OpenGL et fixer les paramètres GL_TEXTURE_MIN_FILTER et GL_TEXTURE_MAG_FILTER à GL_LINEAR pour ce sampler
- Récuperer la location de l'uniform *sampler2D uKdSampler*

Au rendu

- Binder la texture sur la texture unit 0
- Binder le sampler sur la texture unit 0

Fonctions GL à utiliser:

A l'initialisation:

| Sans DSA | DSA |
| ------------------------------------ | ----------- |
| glActiveTexture(GL_TEXTURE0) | |
| glGenTextures | glCreateTextures |
| glBindTexture(GL_TEXTURE_2D, texID) | |
| glTexStorage2D | glTextureStorage2D |
| glTexSubImage2D | glTextureSubImage2D |
| glGenSamplers | glCreateSamplers |
| glSamplerParameteri | glSamplerParameteri |

Au rendu:

| Sans DSA                             | DSA |
| ------------------------------------ | ----------- |
| glActiveTexture(GL_TEXTURE0) |  |
| glUniform1i(location, 0) | glUniform1i(location, 0) |
| glBindSampler(0, samplerID) | glBindSampler(0, samplerID) |
| glBindTexture(GL_TEXTURE_2D, texID) | glBindTextureUnit(0, texID) |