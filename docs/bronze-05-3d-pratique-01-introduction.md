---
id: bronze-05-3d-pratique-01-introduction
title: Introduction
---

Nous allons dans un premier temps dessiner des objets en 3D sans nous occuper de la caméra. Cela revient à utiliser comme **ViewMatrix** la matrice identité. L'objectif de ce TP sera donc de calculer des **ModelMatrix** de manière à dessiner nos objets en 3D à la bonne position.

Pour cela rien de mieux que de dessiner une planête et ses satellites ! En effet, les satellites tournant autour de la planête, il faut gérer la relation de hiérarchie en combinant correctement les transformations.