---
id: gold-01-intro-sdk-05-projet
title: Projet
---

L'objectif du projet est de développer une demo, au sens "Demoscene", c'est à dire une application jouant une animation 3D. [Quelques exemples de demo](https://www.youtube.com/watch?v=gEvVHCg6fHo&list=PL9HVvEQXdWVb_Nakad9URLWoP6sngpWj1).

Le thème imposé est le suivant: **Star Wars** (grosse originalité). Si vous n'aimez pas Star Wars, vous n'aurez qu'à exprimer cette haine dans votre démo, ou le mixer avec un autre thème de votre choix.

En general les elements d'une demo sont procéduraux (générés aléatoirement), mais ce n'est pas obligatoire pour le projet (vous pouvez charger des modèles 3D et textures, vous devriez pouvoir trouver facilement des vaisseaux sur le net). La musique n'est pas obligatoire non plus.

Il est par contre obligatoire de faire une animation: la caméra doit se déplacer seule, et les elements de la scene aussi. Essayez de faire quelque chose d'un minimum artistique. Les interactions avec l'utilisateur sont autorisées mais l'ensemble doit rester majoritairement automatique. L'animation peut boucler ou bien s'arreter et proposer à l'utilisateur de recommencer. La durée minimale d'animation à respecter est d'une minute. Pas de durée maximale.

Votre projet devra être pushé sur votre repository Github.

Le niveau 1 de réalisation du projet est d'utiliser TOUTES les techniques vu en TP pour réaliser votre demo.

Le niveau 2 est l'intégration de techniques avancées pour rendre votre demo encore plus impressionantes. Voici une liste de techniques possible:

- Shadow Volumes
- Normal Mapping (pas suffisant seul, à combiner avec d'autres choses)
- Screen Space Ambient Occlusion
- [Forward +](http://fr.slideshare.net/takahiroharada/forward-34779335)
- [Rendu temps réel d'objets diffus avec une env map grace aux harmoniques spheriques](http://cseweb.ucsd.edu/~ravir/papers/envmap/)
- [Reflective shadow mapping](http://www.klayge.org/material/3_12/GI/rsm.pdf)
- [Imperfect shadow maps](http://resources.mpi-inf.mpg.de/ImperfectShadowMaps/ISM.pdf)
- [Clustered Shading](http://www.cse.chalmers.se/~uffe/clustered_shading_preprint.pdf)
- [Screen Space Ray Tracing](http://jcgt.org/published/0003/04/04/)
- [Virtual Spherical Gaussian Lights for Real-time Glossy Indirect Illumination](http://www.jp.square-enix.com/tech/library/pdf/Virtual%20Spherical%20Gaussian%20Lights%20for%20Real-Time%20Glossy%20Indirect%20Illumination.pdf)

Des sites où trouver d'autres articles:

- http://jcgt.org/read.html?reload=1
- http://kesen.realtimerendering.com/ (en particulier les articles de la conférence I3D)