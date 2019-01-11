---
id: bronze-04-3d-theorie-02-normal-matrix
title: La Normal Matrix
---

Il existe un piège dans tout ce bel univers de transformations: la transformation des normales. Tout d'abord petit rappel: la normale d'un vertex est le vecteur unitaire perpendiculaire à la surface au point considéré. Les normales sont utilisés pour faire des calculs de lumière.

Lorsque l'on effectue des scales non-uniformes (c'est à dire dont le coefficient de scaling est différent sur chacun des axes), alors l'application brute de la transformation sur les normales d'un objet à pour effet de supprimer la relation d'orthogonalité: les normales ne sont alors plus des normales.

Afin de remédier à ce problème on utilise la NormalMatrix définit de la manière suivante:

$$
NormalMatrix=(MV^{−1})T
$$

Pour rappel, \\(MV\\) est la ModelViewMatrix. Pour obtenir la NormalMatrix on l'inverse et on prend la transposée du résultat.

Ainsi, toutes les positions seront transformées en utilisant la ModelViewMatrix (et la ModelViewProjectionMatrix pour **gl_Position**) et toutes les normales seront transformées en utilisant la NormalMatrix.