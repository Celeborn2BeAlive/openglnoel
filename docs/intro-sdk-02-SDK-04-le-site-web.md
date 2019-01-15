---
id: intro-sdk-02-SDK-04-le-site-web
title: Le site web
---

Ce site web est également stocké sur le repository Git. La branche de développement est nommée "www", et la branche de déploiement est nommée "gh-pages" (jetez donc un oeil à [GithHub Pages](https://pages.github.com/) si vous voulez mettre un site en ligne sur Github très facilement).

Cela signifie que grace à la magie du partage, vous pouvez participer à la rédaction des pages que vous êtes en train de lire ! En particulier, je sais que mes fautes d'orthographe écorchent les yeux de certains d'entre vous (ce que je peux comprendre). Pour m'aider à les corriger, rien de plus simple:

- Rendez vous sur la page Github de votre fork du repository
- Passez sur la branche **www**
- Vous verrez deux dossier dans l'arborescence: **docs** et **website**. La majorité du site est rédigée dans **docs**, ouvrez donc ce dossier
- Trouvez le fichier markdown correspondant à la page contenant la faute et ouvrez le
- Vous devriez avoir un petit icone en forme de crayon pour éditer le fichier, cliquez dessus
- Faites votre modification puis cliquez sur **Commit changes** en dessous

Votre modification existe à présent sur votre repository, mais pas sur le mien. Pour que je la récupère, il faut me faire une pull request de la branche. Pour cela:

- Allez dans l'onglet "Pull requests"
- Cliquez sur "New pull request"
- En haut sur les boutons choisisez "base: www" et "compare: www"
- Vous devriez voir apparaitre les changements effectués
- Cliquez sur "Create pull request", vous pouvez me laisser en commentaire si vous voulez dans la boite de texte dediée.

Je devrais voir apparaitre sur mon repository la pull request, que je pourrais merger. Le changement apparaitra sur le site quand je l'aurais déployé.

Merci d'avance pour votre collaboration :)