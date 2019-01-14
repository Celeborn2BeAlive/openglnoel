---
id: silver-03-deferred-renderer-04-plus-loin
title: Aller plus loin
---

Voici plusieurs choses améliorable pour rendre notre deferred renderer plus interessant:

## Compute Shader pour la Shading Pass

Dessiner un quad pour la shading pass, c'est en réalité se compliquer la vie pour rien. Tout ce qu'on veut c'est traiter chacun des pixels du GBuffer, en parallèle sur GPU. D'une certaine manière, ce qu'on aimerait faire c'est un genre de kernel Cuda.

Ca tombe bien, les compute shaders (dispo depuis OpenGL 4.3) permettent exactement de faire ça.

Trouvez vous un tuto sur le net sur les compute shaders et essayez d'adapter votre shading pass pour utiliser un CS plutot que dessin quad + VS + FS.

## Light Culling simple

Il est assez facile en deferred de faire le rendu de la contribution de plusieurs lights en accumulant le résultat de plusieurs shading pass.

De plus, avec des lights dont l'interaction est limités dans l'espace (point light avec un radius par exemple), il est possible de remplacer le dessin du Quad par une forme quelquonque qui vient couvrir en espace ecran la forme du volume projeté de la light. On traite ainsi moins de pixels a chaque shading pass et on gagne du temps. Les [parties 2 et 3](http://ogldev.atspace.co.uk/www/tutorial36/tutorial36.html) du tutorial d'ogldev détaillent cette méthode. Implémentez là.

>  Ces deux exercices sont un peu antagonistes. Dans le premier on remplace le dessin du Quad par l'execution d'un compute shader.
Dans le deuxième on remplace le dessin du Quad par des dessins de différents formes.

## Conclusion et Forward+

Ces deux premiers TPs vous ont permis d'implémenter les deux pipelines de rendu temps réel les plus utilisé: forward et deferred.

Ils ont tous les deux des avantages et inconvénients:

### Forward

Avantages:
- Simple a mettre en oeuvre, un seul programme GLSL
- Accès aux propriétés de l'objet en cours de rendu au moment de l'illumination via les uniforms
- Possibilité d'exploiter les techniques d'anti-aliasing du GPU

Désavantages:
- Potentiellement beaucoup de fragment traités puis occultés
- Pas d'information globale sur l'ensemble de la géométrie visible (en tout cas pas sans précalcul)

### Deferred

Avantages:
- Possibilité d'utiliser le GBuffer pour faire du light culling, post-process ou autre
- Shading pass seulement sur les fragments non occultés -> gain de perfs si shading pass couteuse (souvent le cas)

Désavantages:
- Gourmand en mémoire et bande passante (grosse résolution = grosses textures à sortir, vive la 4K et la VR !)
- Pour avoir accès a des informations sur les objets à rendre dans la shading pass, il faut les écrire dans des textures -> difficile de mélanger différent modèles de shading sur un meme rendu
- Par defaut, impossible de rendre des objets transparents

### Forward+

Une autre méthode a été développé par AMD, appelé Forward + ([voir ici](http://www.gdcvault.com/play/1016435/Forward-Rendering-Pipeline-for-Modern) et [la](http://fr.slideshare.net/takahiroharada/forward-34779335)), qui tente d'obtenir les avantages des deux méthodes sans les inconvénients.

Sans trop entrer dans les détails, voici le fonctionnement général de l'algo:

- depth pré-pass: rendu uniquement du depth buffer
- light culling: utilisation du depth buffer pour associer les lights à des tiles de pixels
- lighting: rendu en forward, mais utilisation du depth buffer précalculé pour discard les fragments occultés, + utilisation des lights associés aux tiles de pixels