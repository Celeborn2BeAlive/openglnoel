---
id: silver-04-shadow-mapping-04-aller-plus-loin
title: Aller Plus Loin
---

Ces exercices vous ont montré comment implementer le **shadow mapping** pour une **lumière directionnelle**.

L'implémentation est un peu plus compliquée pour une **lumière ponctuelle** car il faut pouvoir projeter sur la carte de profondeur l'ensemble de la scene vu depuis la point light, c'est à dire la considérer comme une camera pouvant voir à 360°.

Le plus simple et robuste pour cela est de faire 6 rendus de la scene sur les faces d'un cube entourant la point light. La branche cheat contient l'application "shadow-mapping-03-point-light-sm" qui implémente cette méthode. Vous pouvez aller lire le code si vous êtes interessé, ou mieux essayer de l'implémenter vous même.

Le shadow mapping est un algorithme simple à comprendre, mais assez peu robuste dans sa version initiale (artefacts tel que shadow acné, ou apparition des pixels de la shadow map). Pour autant, Il reste la méthode d'ombrage la plus utilisée dans les jeux vidéo. Cela est rendu possible par les nombreuses améliorations dont il a bénéficié grâce à la recherche.

Pour finir, voici deux bons livres de référence sur les méthodes d'ombrages:

- [Shadow Algorithms Data Miner](https://www.amazon.com/gp/product/1439880239?tag=realtimerenderin&pldnSite=1)
- [Real-Time Shadows](https://www.amazon.com/Real-Time-Shadows-Michael-Wimmer/dp/1568814380?tag=realtimerenderin&pldnSite=1)