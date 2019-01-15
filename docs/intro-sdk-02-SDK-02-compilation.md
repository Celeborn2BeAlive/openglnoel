---
id: intro-sdk-02-SDK-02-compilation
title: Compilation du SDK
---

Le repository que vous avez forké et cloné contient un SDK pour coder les TDs. Cette partie explique comment compiler l'ensemble du SDK simplement.

## Compilation

- Avec CMake
- Support de GCC 5+ (sans boost), GCC 4.9.2 (boost requis) et Visual Studio 2017 (sans boost).

CMake est un logiciel permettant de générer une solution de compilation adaptée au système sur lequel on l'execute. Par exemple sous Linux il pourra générer un makefile, sous Windows une solution Visual Studio, etc. CMake permet donc de créer un projet multi-plateforme assez facilement. C'est devenu un standard et la plupart des projets Open Source fournissent un fichier CMakeLists.txt.

J'ai mis des scripts dans le SDK afin de lancer CMake sur le fichier CMakeLists.txt avec les bonnes options. Le lancement de ces scripts créé un repertoire **openglnoel-build** a coté du repository cloné. Ce repertoire contient un Makefile sous linux, ou une solution Visual Studio sous windows.

### Linux

Sous Linux la procédure est un peu différente selon la version de GCC, à cause de mon utilisation de la lib standard experimentale **filesystem** qui n'est pas disponible avant GCC 5. Pour vérifier la version de GCC, utilisez la commande:

```bash
gcc -v
```

Ensuite en console:
1. Se placer dans le repertoire du repository
2. Utilisez la commande **bash scripts/linux/generate_gcc_solution.sh** si GCC 5+ est installé, sinon utilisez **bash scripts/linux/generate_gcc_boost_solution.sh** (installez boost si necessaire, sur les postes de la FAC c'est déjà fait).
3. Un nouveau repertoire **openglnoel-build** a normalement été créé à coté du repository cloné, placez vous dedans en console
4. Lancez la commande **make -j** pour compiler la solution
5. Lancer l'application **bin/app-template** pour tester

Tous les executables seront compilés dans le repertoire *bin* du dossier de build.

### Windows

1. Télécharger et installer [CMake GUI](https://cmake.org/download/) (Windows win64-x64 Installer ou Windows win32-x86 Installer selon votre CPU) et [Visual Studio Community Edition 2017](https://www.visualstudio.com/fr/vs/community/). A l'installation, répondre "oui" lorsqu'il vous demande si vous voulez ajouter CMake à votre variable d'environnement PATH.
2. Lancer le script /scripts/windows/generate_vs2017_solution.bat (double cliquez dessus).
3. Un nouveau repertoire **openglnoel-build** a normalement été créé à coté du repository cloné, ouvrez le
4. Ouvrez la solution Visual Studio **openglnoel.sln**
5. Dans le panel *Explorateur de solutions*, choisir le projet *app-template* comme projet de démarage (clic droit dessus, puis trouver l'option)
6. Menu *Générer* -> *Générer la solution*
7. Lancer l'application avec F5 (debug) ou Ctrl+F5 (non debug)

Les executables sont compilés dans les repertoire *bin/Debug* et *bin/Release* du repertoire de build, selon la configuration choisie dans Visual Studio.

## Installer la version compilée

Il est possible d'installer les applications compilée. Les scripts font en sorte que tout soit installé dans le repertoire **openglnoel-install**, à coté du repository cloné.

Sous linux il faut lancer la commande suivante dans le repertoire de build:

```bash
make -j install
```

Sous windows il faut générer le projet *INSTALL* de la solution afin que les executables compilés soit recopiés dans un repertoire d'installation.

## Recréer la solution

Il vous sera parfois necessaire de relancer les scripts, par exemple lors de l'ajout de fichiers sources dans votre projet. Pour cela pas besoin de supprimer le repertoire de build, relancez juste les scripts selon les procédures indiquées.

Si la compilation se met à échouer sans raison logique, alors vous pouvez essayer de supprimer le repertoire de build et de relancer le script.