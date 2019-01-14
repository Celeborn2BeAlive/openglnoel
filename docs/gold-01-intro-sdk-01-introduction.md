---
id: silver-01-intro-sdk-01-introduction
title: Introduction
---

## Objectifs

- Approfondir les connaissances en OpenGL Moderne
- Premières implémentations de techniques avancées
- **Projet**: Demo

## Prérequis

- Bonne connaissance du [pipeline de rendu OpenGL](https://www.khronos.org/opengl/wiki/Rendering_Pipeline_Overview)
- Maitrise des [TDs des années précédentes](http://laurentnoel.fr/index.php?section=teaching&teaching=opengl&teaching_section=tds)
- Maitrise de base du C++

## Quelle version d'OpenGL ?

La version installée sur les machines de la fac est la 4.4, nous prendrons donc cette version comme base.

En plus de cela, et si votre carte graphique le permet, il vous sera possible de travailler avec l'extension **GL_ARB_direct_state_access** (DSA) qui simplifie grandement la manipulation d'objets OpenGL (pas besoin de binder les objets pour les manipuler). A noter que les PCs de la fac ne permette pas l'utilisation de cette extension (en partie, mais pas à 100%).

Si vous êtes interessé par l'utilisation de cette extension, consultez [la section dediée sur ce site](/06-course/opengl-extensions/#direct-state-access).

La plupart des sujets de TDs fournissent la liste des fonctions à utiliser en DSA.

Pas d'inquiétude si votre carte graphique n'est pas équipée de l'extension: tous les sujets fournissent aussi la version des fonctions à utiliser sans DSA.

## Planning

Nous aurons 5 x 4h ensemble:

- Lundi 29 / 01 8h30 - 12h45: Fork du repo github, mise en place du SDK, fiche d'info perso, début du Forward Renderer
- Vendredi 02 / 02 8h30 - 12h45: Fin du Forward Renderer
- Lundi 05 / 02 8h30 - 12h45 4h: Deferred Renderer
- Lundi 05 / 02 14h - 18h15: Shadow Mapping
- Vendredi 09 / 02 8h30 - 12h45: Gouter de fin d'année (et post processing si on à le temps)