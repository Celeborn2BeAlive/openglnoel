---
title: Fin du portage des TDs
author: Laurent NOËL
authorURL: http://laurentnoel.fr
authorFBID: 100003593998838
---

J'ai terminé de porter les TDs Imac 2 et Imac 3 / Master 2:
- Imac 2 dans Bronze Level
- Imac 3 / Master 2 dans Silver Level

Mon prochain objectif est d'homogénéiser ces deux parties qui sont actuellement basées sur deux templates de code assez différent. Je pense également à modifier le découpage:
- Bronze Level serait limité à OpenGL 3+ appliqué à la 2D
- Silver Level serait l'application à la 3D, avec forward rendering, blinn-phong et lumières simples
- Gold Level serait des techniques avancées en 3D: deferred rendering, shadow mapping, PBR materials et lighting, post processing

De cette manière, le programme Imac 2 / Master 1 sera de suivre Bronze Level et Silver Level, et le programme Imac 3 / Master 2 sera de suivre Silver Level et Gold Level.

La partie **Introduction et SDK** est destinée à présenter le site, OpenGL 3+, le template de code ainsi que les outils annexes (CMake, Github).

Au niveau du template de code, il sera stocké sur la branche master du repository de ce site et sera basé sur les libs suivantes:
- GLFW pour le fenetrage et les interactions
- glad pour le chargement d'OpenGL 3+
- imgui pour la GUI
- glm pour les maths
- assimp pour le chargement de fichiers 3D
- Une lib pour charger des modèles glTF (probablement tiny gltf loader)
- nlohmann json pour charger des fichiers json

Coté compilation, le template proposera un CMake permettant de rapidement ajouter de nouvelles applications et bibliothèques. Je mettrais également des scripts .bat et .sh pour la générations de solutions pré-configurées. L'objectif a terme est vraiment de pouvoir forker le repo, le cloner, lancer un script, puis coder.

<!--truncate-->