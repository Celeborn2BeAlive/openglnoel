---
id: bronze-04-3d-theorie-04-glm
title: La bibliothèque glm
---

Le calcul des matrices se fera dans le code C++. On enverra ensuite à chaque tour de la boucle principale les matrices MV, MVP et NormalMatrix au shader en utilisant des variables uniformes. On pourrait s'amuser à recoder toute une bibliothèque de gestion de vecteur / matrices mais, heureusement, d'autres l'ont fait pour nous :D

Nous allons utiliser la bibliothèque **glm**. Celle ci a pris le partit d'utiliser les même nom de types / fonctions que le langage GLSL. Ainsi le type **glm::vec3** est sémantiquement identique au type **vec3** de GLSL. De cette manière vous n'aurez quasiement rien à apprendre de plus.

[La documentation en ligne](https://glm.g-truc.net/0.9.9/api/modules.html) donne une description de chacune des fonctions. Quand le mot clef **genType** est utilisé, cela représente n'importe quel type numérique. Par exemple sur la page [Common functions](https://glm.g-truc.net/0.9.9/api/a00662.html), les lignes:

```cpp
template<typename genType>
genType abs(genType const &x);
```

indiquent qu'il existe une fonction **glm::abs** pour calculer la valeur absolue de n'importe quel nombre quel que soit son type numérique (int, float, double, mais également les types vecteurs comme **glm::vec3**).