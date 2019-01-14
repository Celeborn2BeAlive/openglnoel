---
id: gold-01-intro-sdk-02-repo-github
title: Repository Github
---

## Git ?

Si vous ne connaissez pas, [Git est un système distribué de gestion de version](https://git-scm.com/): un logiciel permettant d'enregistrer au fur à mesure les modifications du code source d'un projet, de manière partagé entre plusieurs utilisateur.

On peut ainsi facilement revenir en arrière, partager du code, controler qui fait quoi, gérer plusieurs versions, etc.

L'objectif de ces TDs n'est pas d'apprendre à utiliser Git, mais il sera pratique pour moi de suivre votre progression et de récupérer votre projet grace à un repo Git publique.

## Compte Github

Si ce n'est pas déjà fait, commencez par créer un compte sur [Github](https://github.com/). Ce site vous permet de créer des repository Git et de contribuer sur des projets Open Source.

N'oubliez pas de vérifier votre e-mail en cliquant sur le lien reçu.

## Fork du repo des TDs

Une fois connecté sur Github, allez sur la page du repository des TDs https://github.com/Celeborn2BeAlive/opengl-avance et cliquez sur le bouton "Fork" en haut à droite.

Cela va créer une copie de mon repository "opengl-avance" sur votre compte Github. Vous pourrez utiliser cette copie comme bon vous semble, faire des modifications (c'est le but !), et même me soumettre vos modifications pour intégration dans le repository original (à éviter à moins que je vous le demande).

## Installer Git

Sur les machines de la Fac Git est déjà installé. Sous Linux il suffit d'utiliser votre gestionnaire de paquet. Par exemple sous Ubuntu (versions récentes):

```bash
sudo apt install git
```

Commande à adapter à votre distribution Linux.

Sous windows [téléchargez et installez Git](https://git-scm.com/download/win). Vous devriez ensuite pouvoir lancer une console Git (recherchez "Git" à partir du menu windows) et executer les commandes à venir, comme sous linux.

## Clonage de votre repo

Il faut à présent cloner en local votre copie du repository. On doit pour cela utiliser la commande **git clone**. Dans une console, commencez par vous placer dans le repertoire de votre choix (utilisez la console Git sous windows):

```bash
cd /le/repertoire/de/mon/choix
```

On clone ensuite le repo (c'est évident mais j'ai appris à me méfier: remplacez *\<username\>* par votre nom d'utilisateur Github):

```bash
git clone https://github.com/<username>/opengl-avance
```

Si vous travaillez en binome, choisissez l'un des deux repo pour travailler en TD.

Un nouveau dossier **openglnoel** devrait alors être créé. Cela ci contient le repository local. Déplacez vous dedans en console.

## Branches du repo

Un repository peut être composé de plusieurs branches, c'est à dire différente versions du code source, à différent états d'avancement, voir des branches avec un contenu completement différent des autres. Voici les branches du repo de TD:

Principales:

- master *Code de base + apps d'exemple en OpenGL 4.4*
- master-direct-state-access *Code de base + apps d'exemple en OpenGL 4.5 (avec DSA)*

Corrections:

- cheat *Correction des TDs en en OpenGL 4.4. A utiliser lorsque vous aurez perdu toute dignité*

Site web des TDs (ne devrait pas vous concerner):

- hugo-website *Site web statique développé avec Hugo*
- gh-pages *Version release du site web, que vous etes en train de lire en ce moment*

Inutiles pour vous:

- voxskel *Un truc que j'avais codé pour débugguer un code*

Pour changer de branche il faut utiliser la commande 

```bash
git checkout <branch_name>
```

Vous n'aurez normalement pas à changer de branche, mais si vous voulez travailler en direct state access, passez sur la branche *master-direct-state-access*.

## Première modification

Commençons par une modification simple: l'ajout d'un fichier *README.md*. Créez ce fichier et écrivez ce que vous voulez dedans.

Utilisez ensuite la commande:

```bash
git status
```

Git vous informe alors des changements dit "non commités" dans votre repo.

Lorsque l'on ajoute un nouveau fichier, il faut le dire à git. Pour cela on utilise la commande:

```bash
git add <nom_du_fichier>
```

Faites le sur README.md.

Si vous utilisez à nouveau *git status*, vous verrez que le message à changé.

Il faut ensuite dire à Git de "commiter" nos modifications sur les fichiers, grace à la commande:

```bash
git commit -a -m "Message de commit décrivant les modifications effectué"
```

Utilisez cette commande avec le message "Add README.md".

A nouveau *git status* donne un nouveau message, indiquant que notre repository à un commit d'avance sur origin/master, qui correspond au repo sur Github.

Il faut donc "pusher" nos modifications sur Github:

```bash
git push
```

Git devrait alors vous demander votre username/password pour vous autoriser à push. Vous pourrez ensuite voir vos modifs sur votre repo en ligne, en particulier un fichier README.md s'affiche en page d'accueil du repository (ce fichier doit etre ecrit au format Markdown pour un rendu correct).

> Pour eviter d'avoir à entrer username/password à chaque push, il est possible de passer par une clef ssh et utiliser l'url en git:// de votre repo. Consulter <a href="https://help.github.com/articles/connecting-to-github-with-ssh/">cette page pour plus d'info</a>.

## Conseils pour la gestion du repo

Afin d'exploiter au mieux votre repo, je vous conseiller de commiter vos changement assez souvent afin de "sauvegarder" votre code. Pensez à pusher quelques fois par TDs afin d'avoir vos changement en ligne et que je puisse les consulter.
