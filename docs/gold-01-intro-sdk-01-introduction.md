---
id: gold-01-intro-sdk-01-introduction
title: Introduction
---

## Objectifs

- Approfondir les connaissances en OpenGL Moderne
- Implémentations de techniques avancées

## Prérequis

- Bonne connaissance du [pipeline de rendu OpenGL](https://www.khronos.org/opengl/wiki/Rendering_Pipeline_Overview)
- Maitrise des niveaux Bronze et Silver
- Maitrise de base du C++

## Quelle version d'OpenGL ?

La version installée sur les machines de la fac est la 4.4, nous prendrons donc cette version comme base.

En plus de cela, et si votre carte graphique le permet, il vous sera possible de travailler avec l'extension **GL_ARB_direct_state_access** (DSA) qui simplifie grandement la manipulation d'objets OpenGL (pas besoin de binder les objets pour les manipuler). A noter que les PCs de la fac ne permette pas l'utilisation de cette extension (en partie, mais pas à 100%).

Si vous êtes interessé par l'utilisation de cette extension, consultez [la section dediée sur ce site](gold-06-course-03-opengl-extensions#direct-state-access).

La plupart des sujets de TDs fournissent la liste des fonctions à utiliser en DSA.

Pas d'inquiétude si votre carte graphique n'est pas équipée de l'extension: tous les sujets fournissent aussi la version des fonctions à utiliser sans DSA.

## Le SDK

<span class="badge todo"></span> Si ce n'est pas déjà fait, récupérez et compilez le SDK en suivant [la procédure indiquée de la partie dédiée](intro-sdk-02-SDK-01-repository)