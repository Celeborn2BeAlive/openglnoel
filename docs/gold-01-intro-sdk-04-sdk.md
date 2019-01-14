---
id: gold-01-intro-sdk-04-sdk
title: Software Development Kit
---

Le repository que vous avez forké et cloné contient un SDK pour coder les TDs. Cette partie explique comment compiler les applications d'exemple, vos futures applications, la structure du repertoire et les spécifité du fichier CMake.

## Compilation

- Avec CMake
- Support de GCC 5+ (sans boost), GCC 4.9.2 (boost requis) et Visual Studio 2015 (sans boost) (devrait aussi fonctionner avec Visual 2017, mais je n'ai pas encore testé)

> CMake est un logiciel permettant de générer une solution de compilation adaptée au système sur lequel on l'execute. Par exemple sous Linux il pourra générer un makefile, sous Windows une solution Visual Studio, etc. CMake permet donc de créer un projet multi-plateforme assez facilement. C'est devenu un standard et la plupart des projets Open Source fournissent un fichier CMakeLists.txt.

### Linux

En console:

1. Créer un repertoire *opengl-avance-build* à coté du repertoire *opengl-avance* (correspondant au clone du repo)
2. Se placer dans *opengl-avance-build*
3. Sur un PC ou GCC 5+ est installé, taper la commande **cmake ../opengl-avance**. Si uniquement GCC 4.9.2 est disponible (machines de la FAC), taper la commande **cmake -DGLMLV_USE_BOOST_FILESYSTEM=ON ../opengl-avance**
4. Compiler avec **make -j**

Les executables sont compilés dans le repertoire *bin* du dossier de build

### Windows

1. Télécharger et installer [CMake GUI](https://cmake.org/download/) (Windows win64-x64 Installer ou Windows win32-x86 Installer selon votre CPU) et [Visual Studio Community Edition 2015](https://www.visualstudio.com/fr/vs/community/).
2. Créer un repertoire *opengl-avance-build* à coté du repertoire *opengl-avance* (correspondant au clone du repo)
3. Lancer CMake GUI, choisir comme repertoire source le clone du repo et comme repertoire de build le dossier que vous avez créé
4. Cliquer sur **Configure** et choisir comme generateur: *Visual Studio 14 2015 Win64* pour un CPU 64-bits, *Visual Studio 14 2015* pour un CPU 32-bits (ça existe encore ?)
5. Cliquer sur **Generate**
6. Ouvrir la solution Visual Studio *opengl.sln* qui a été créée par CMake dans le repertoire de build
7. Dans le panel *Explorateur de solutions*, choisir le projet *template* comme projet de démarage (clic droit dessus, puis trouver l'option)
8. Menu *Générer* -> *Générer la solution*
9. Lancer l'application avec F5 (debug) ou Ctrl+F5 (non debug)

Les executables sont compilés dans les repertoire *bin/Debug* et *bin/Release* selon la configuration.

Il est possible de compiler le projet *INSTALL* de la solution avec que les executables compilés soit recopiés dans un repertoire d'installation. Par défault ce repertoire est *C:/Program Files/opengl* mais il est possible de le changer depuis CMake GUI en recherchant la variable *CMAKE_INSTALL_PREFIX* et en la modifiant.

## Arborescence

- apps *Contient le code des executables à compiler*
- cmake *Contient des modules cmake*
- lib *Contient le code partagé entre les executables*
- third-party *Contient des bibliothèques externes*

Chaque sous-repertoire de *apps* est compilé en tant qu'application indépendante. Ainsi le code du repertoire *apps/template* sera compilé en un executable *template* (Linux) ou *template.exe* (Windows), et le code du repertoire *apps/triangle* sera compilé en un executable *triangle* (Linux) ou *triangle.exe* (Windows).

Le répertoire *lib* est destiné à contenir du code d'une lib (nommée *glmlv*) partagée entre les executables.

Afin de garder un code clair, je vous conseille de créer une application différente par theme de TP. Il faudra également créer une application pour le projet.

## Applications d'exemple

Le repertoire *apps* contient déjà du code pour plusieurs applications simples. Toutes sont basées sur le template de base *template* qui se contente d'ouvrir une fenêtre contenant une GUI affichant le framerate et permettant de changer la couleur de fond.

Ces codes simples sont destinés à vous donner quelques exemple d'appels aux fonctions OpenGL, en particulier l'utilisation des fonction de l'extension direct_state_access sur la branche *master-direct-state-access*.

Une ressource supplémentaire est [ce repository GIT](https://github.com/g-truc/ogl-samples/) contenant un grand nombre d'exemples de code pour l'ensemble des features OpenGL.

Voici une description des apps d'exemple:

- triangle *Dessine un triangle coloré* (création de VBO, création de VAO, attributs entrelacés, chargement de shaders)
- triangle_2vbos *Pareil mais en utilisant 2 VBOs, un pour les positions, l'autre pour les couleurs* (plusieurs VBOs pour un objet, attributs non entrelacés)
- quad *Dessine un quad coloré* (création d'IBO, i.e. buffer d'index)

### Gestion des shaders

Concernant les shaders GLSL, le processus de compilation se charge de les copier à coté des executables afin qu'il soient facilement accessible (de manière relative) par ces derniers. Leur extension doit être ".glsl" afin qu'ils soient reconnu par le CMakeLists.txt, et ils doivent être placés dans un sous repertoire *shaders* de l'application qui les utilise. Par convention, on suffixera ".vs.glsl" les vertex shader, ".fs.glsl" les fragments shaders, ".gs.glsl" les geometry shaders et ".cs.glsl" les compute shaders.

La lib *glmlv* peut également contenir des shaders partagés entre les applications, en les plaçant dans le repertoire *lib/shaders*.

Par exemple, supposons l'arborescence suivante pour les shaders de *apps* et *libs*:

- apps
    - triangle
        - shaders
            - triangle.vs.glsl
            - triangle.fs.glsl
    - smab
        - shaders
            - shadow_mapping
                - sm.vs.glsl
                - sm.fs.glsl
            - deferred
                - gbuffer.vs.glsl
                - gbuffer.fs.glsl
                - shading.vs.glsl
                - shading.fs.glsl
- lib
    - shaders
        - post-processing
            - blur.cs.glsl
            - dof.cs.glsl

On a ici deux applications *triangle* et *smab*, contenant chacune des shaders différents et partageant deux shaders de *glmlv*.

Cette arborescence sera reproduite de la manière suivante dans le repertoire de build:

- opengl-avance-build
    - bin
        - triangle.exe
        - smab.exe
        - shaders
            - glmlv
                - post-processing
                    - blur.cs.glsl
                    - dof.cs.glsl
            - triangle
                - triangle.vs.glsl
                - triangle.fs.glsl
            - smab
                - shadow_mapping
                    - sm.vs.glsl
                    - sm.fs.glsl
                - deferred
                    - gbuffer.vs.glsl
                    - gbuffer.fs.glsl
                    - shading.vs.glsl
                    - shading.fs.glsl

De cette manière, un code comme le suivant accède facilement aux shaders **sans dépendre de l'emplacement de ces derniers sur la machine de l'utilisateur**.

```cpp
#include <glmlv/filesystem.hpp>

// Supposons que le code est celui de smab.exe, dont le chemin est C:/toto/tata/bin/smab.exe
int main(int argc, char** argv)
{
    // argv[0] contient toujours le chemin vers l'executable, ici "C:/toto/tata/bin/smab.exe"
    // On le récupère dans une variable de type glmlv::fs::path permettant de gérer facilement les chemins
    const auto applicationPath = glmlv::fs::path{ argv[0] };

    // stem() renvoit le nom du fichier sans son extension, i.e. "smab":
    const auto appName = applicationPath.stem().string();

    // L'opérateur '/' permet de concatener les chemins de fichier
    const auto shadersRootPath = applicationPath.parent_path() / "shaders";

    // Pour les shaders de l'application, on utilise la variable contenant le
    // nom de l'application pour accéder au dossier contenant ses shaders
    const auto pathToSMVS = shadersRootPath / appName / "shadow_mapping" / "sm.vs.glsl";
    const auto pathToSMFS = shadersRootPath / appName / "shadow_mapping" / "sm.fs.glsl";

    // Pour les shaders de la lib, on cible directement le dossier "glmlv"
    const auto pathToBlurCS = shadersRootPath / "glmlv" / "post-processing" / "blur.cs.glsl";

    [...]

    return 0;
}
```

## Lib interne (glmlv)

La bibliothèque interne, dont le code est situé dans le repertoire *lib* du template, est destiné à contenir le code partagé entre les applications. **Les classes et fonctions réutilisables doivent donc y être placées**.

Le template de base contient déjà plusieurs fichiers dans la lib:

- filesystem.hpp *Inclut une bibliothèque de manipulation du système de fichier (soit la lib experimentale de la std, soit la lib de boost si cmake a été appelé avec l'option -DGLMLV_USE_BOOST_FILESYSTEM)*
- gl_debug_output.cpp/hpp *Définit la fonction initGLDebugOutput() permettant d'activer les message de debug d'OpenGL*
- glfw.hpp *Inclut simplement les header de GLFW et glad dans le bon ordre afin d'éviter des erreurs de compil*
- GLFWHandle.hpp *Définit la classe GLFWHandle qui initialise GLFW, ouvre une fenetre, initialise OpenGL avec glad et initialise ImGUI dans son constructeur*
- GLProgram.hpp, GLShader.hpp *Définissent des classes et fonction helpers pour charger, compiler et linker des shaders GLSL*
- Image2DRGBA.hpp *Définit la classe Image2DRGBA et des fonctions de lecture/écriture d'image en surcouche de la lib stb*
- imgui_impl_glfw_gl3.hpp/.cpp *Définit les fonctions de dessin et d'interaction avec l'utilisateurs nécessaires au bon fonctionnement de ImGui*

## Libs externes (third-party)

Le template contient plusieurs bibliothèques externes (dans *third-party*) afin de vous simplifier la vie:

- [glfw](http://www.glfw.org/): bibliothèque de fenetrage et de gestion d'inputs (similaire à la SDL)
- [glm](http://glm.g-truc.net/): bibliothèque de maths "à la GLSL"
- [glad](http://glad.dav1d.de/): bibliothèque pour "charger" les fonctions OpenGL 3+ (similaire à GLEW)
- [imgui](https://github.com/ocornut/imgui): bibliotheque permettant d'afficher une interface utilisateur de manière simple et en "immediate mode"
- [json](https://github.com/nlohmann/json): bibliotheque pour lire/écrire des fichiers json, pratique pour gérer rapidement des fichiers de config
- [tinyobjloader](https://github.com/syoyo/tinyobjloader): bibliotheque pour charger des fichiers OBJ (format simple de scenes 3D)
- [stb](https://github.com/nothings/stb) contient les header *stb_image.h* et *stb_image_write.h* de la bibliothèque stb, afin de lire et écrire des images.

Mon objectif n'étant pas de vous apprendre à utiliser ces libs, je détaillerais assez peu leurs fonctionnalités dans les TPs. Les liens ci-dessus redirigent vers des documentations assez complètes pour que vous puissiez vous débrouiller :)

A noter que ImGui fournie une fonction ImGui::ShowTestWindow dont le code montre des exemples d'utilisation d'a peu près tous les widgets de la lib. Le code de cette fonction est dans le fichier *imgui_demo.cpp* du repertoire contenant la lib.