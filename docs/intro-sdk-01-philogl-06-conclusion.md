---
id: intro-sdk-01-philogl-06-conclusion
title: Conclusion
---

## Quelques conseils pour survivre en OpenGL 3

Coder en OpenGL 3 n'est pas si compliqué, c'est simplement long et fastidieux. On y prend néammoins énormément de plaisir lorsqu'on commence à avoir une bonne comprehension du fonctionnement du pipeline et du GPU. Voici quelques conseils pour éviter de trop souffrir au début :p

- Garder son code organisé selon le schéma de l'application multimédia. Essayer de regrouper ensemble tous les appels de fonctions effectuant une tache similaire.
- Factoriser le code redondant dans des fonctions et des classes que vous placerez dans la bibliothèque glimac.
- Eviter de cacher trop vite l'aspect bas niveau d'OpenGL: il est nécessaire de bien pratiquer et comprendre ce qu'il se passe au niveau du pipeline avant d'introduire trop d'abstraction.
- OpenGL possède beaucoup d'états globaux. Essayer dans la mesure du possible de savoir, à tout point du code, quelle est la valeur de ces états.
- Lire beaucoup au sujet d'OpenGL. En particulier la partie Liens de cette section fournit plusieurs tutoriaux et explications sur le fonctionnement d'OpenGL. Il ne faut pas hésiter à passer un peu de temps à potasser tout ça, même sans tout comprendre au début. C'est à force de pratiquer et lire qu'on commence à avoir les idées claires.
- Garder un esprit critique sur tous les exercices réalisés. On a souvent tendance à appliquer ce qui est demandé dans les TPs sans vraiment comprendre ce que l'on fait. Avec OpenGL ça ne peut pas marcher: il faut vraiment bien comprendre l'effet de chaque ligne que l'on écrit car elle aura potentiellement un effet global sur tout votre code.

Pour l'instant tous ces conseils peuvent sembler abstraits. Revenez y dans quelques TPs :)

## Conclusion

Pour résumer:

- L'API OpenGL a radicalement évoluée entre les version 2 et 3 pour réfleter les nouvelles fonctionnalités offertes par les GPUs.
- Il est plus difficile de faire des choses simples en OpenGL 3 mais plus facile de faire des choses compliqués :)
- Les deux gros changement par rapport à OpenGL 2 sont la suppression du rendu direct et du pipeline fixe.
- Le core profile permet de savoir quelles fonctions ne sont pas dépréciées.