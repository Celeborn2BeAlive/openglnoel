---
id: silver-04-lighting-01-introduction
title: Introduction
---

Ce dernier TP sera l'occasion d'implanter un modèle d'éclairage simple permettant de gérer des lumières de type ponctuelle ou directionelle, sans ombres.

Le modèle que nous allons mettre en place est le modèle de Blinn-Phong. C'est le fragment shader qui se chargera de faire le calcul d'éclairage.

![Blinn Phong](/openglnoel/img/blinnPhong.svg)

Le modèle de Blinn-Phong est définit par l'équation suivante:

$$
Couleur=Li(Kd(wi \cdot{} N)+Ks(halfVector \cdot{} N)^{shininess})
$$

où:

- \\(wi\\) est la direction incidente depuis laquelle provient la lumière. On la calcule par rapport à la position de cette dernière.
- \\(Li\\) est la lumière (une couleur) incidente dans la direction wi. On la calcule par rapport à l'intensité et la distance à la lumière.
- \\(N\\) est la normale.
- \\(Kd\\) est le coefficient de reflection diffuse de l'objet. C'est une couleur qui dépend du materiau de l'objet.
- \\(Ks\\) est le coefficient de reflection glossy de l'objet. C'est une couleur qui dépend du materiau de l'objet.
- \\(shininess\\) est un exposant de brillance permettant de controller la taille de la "tache" de brillance glossy. C'est un float qui dépend du materiau de l'objet.
- \\(halfVector\\) est le vecteur \\(\frac{wo+wi}{2}\\), wo étant le vecteur pointant vers la caméra.

Dans la formule, tous les produits ˙ entre vecteurs sont des produits scalaire. Les vecteurs doivent tous être normalisés.

La notion de **matériau** est très importante en synthèse d'image. Nous allons utiliser des variables uniformes pour passer les coefficients \\(Kd\\), \\(Ks\\) et \\(shininess\\) au shader avant de dessiner. Cela aura pour effet de dessiner des objets dont le matériau est le même en chaque point. Dans une application plus complexe on utiliserait le **multi-texturing** pour binder une texture diffuse, une texture glossy et une texture de brillance. De cette manière on peut avoir des coefficients différents pour chaque point d'un objet.