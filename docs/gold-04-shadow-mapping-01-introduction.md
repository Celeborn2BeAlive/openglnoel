---
id: silver-04-shadow-mapping-01-introduction
title: Introduction
---

Nous avons jusqu'a présent calculé l'éclairage des lumières sans prise en compte des ombres portées par les objets: la lumière passe à travers...

Pour savoir si un fragment est dans la lumière ou dans l'ombre par rapport à une light donnée, il faut savoir si un objet intersecte le rayon connectant le fragment à la lumière. Il y a plusieurs moyens de calculer cette information. Deux méthodes sont principalement utilisées en rendu temps réel: le [shadow mapping](https://www.wikiwand.com/en/Shadow_mapping) et les [shadow volumes](https://www.wikiwand.com/en/Shadow_volume).

Nous allons nous intéresser au shadow mapping pour ces TPs. L'idée est de faire un rendu de la scène selon le point de vue de la lumière afin de connaitre l'ensemble des points visibles depuis cette lumière. Plus exactement, on calcule une carte de profondeur (depth map) vue par la lumière. Ensuite, pendant le rendu principal de la scène (depuis le point de vue de la caméra), on projete les points vus par la caméra sur la depth map précalculée pour la light. On obtient ainsi un pixel de la depth map, et si la depth enregistrée dans cette map est inférieure à la depth du fragment vu par la caméra, alors necessairement un objet vient occulter ce fragment, qui est donc dans l'ombre.

Le schéma suivant résume ce principe:

![Shadow Mapping](/images/shadow_mapping.jpg)

Cet algorithme peut être implémenté sur un forward renderer ou sur un deferred renderer. Pour ces TPs nous allons repartir du deferred renderer. Si vous ne l'avez pas terminé, vous pouvez le récupérer sur la branche cheat du repository.

> Afin de faire vos tests plus rapidement, j'ai ajouté un argument à la fonction *glmlv::loadObj* qui permet d'éviter le chargement des textures et donc de lancer plus rapidement l'application (mettre l'argument à *false* pour ne pas charger les textures). Il faudra peut être puller la dernière version du repo pour récupérer ce changement.