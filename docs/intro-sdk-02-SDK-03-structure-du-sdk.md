---
id: intro-sdk-02-SDK-03-structure-du-sdk
title: Structure du SDK
---

## Arborescence

- apps *Contient le code des executables à compiler*
- cmake *Contient des modules cmake*
- lib *Contient le code partagé entre les executables*
- scripts *Contient les scripts pour générer la solution à partir du CMakeLists.txt*
- third-party *Contient des bibliothèques externes*

Chaque sous-repertoire de *apps* est compilé en tant qu'application indépendante. Ainsi le code du repertoire *apps/app-template* est compilé en un executable *app-template* (Linux) ou *app-template.exe* (Windows).

Le répertoire *lib* contient le code d'une lib (nommée *glmlv*) partagée entre les executables, permettant de factoriser certaines parties.

Afin de garder un code clair, je vous conseille de créer une application différente par sujet de TP. Il faudra également créer une application pour le projet.

## L'application *app-template*

Le repertoire *apps* contient déjà le code d'une application *app-template**. Celle ci se contente d'ouvrir une fenetre et d'afficher le framerate dans une sous fenetre. Il y a plusieurs commentaire indiquant ou placer votre code.

Chaque nouvelle application peut être créée rapidement avec un copier-coller-renommage de *app-template*.

## Libs externes (third-party)

Le template contient plusieurs bibliothèques externes (dans *third-party*) afin de vous simplifier la vie:

- [glfw](http://www.glfw.org/): bibliothèque de fenetrage et de gestion d'inputs
- [glm](http://glm.g-truc.net/): bibliothèque de maths "à la GLSL"
- [glad](http://glad.dav1d.de/): bibliothèque pour "charger" les fonctions OpenGL 3+ (similaire à GLEW)
- [imgui](https://github.com/ocornut/imgui): bibliotheque permettant d'afficher une interface utilisateur de manière simple et en "immediate mode"
- [json](https://github.com/nlohmann/json): bibliotheque pour lire/écrire des fichiers json, pratique pour gérer rapidement des fichiers de config
- [assimp](https://github.com/assimp/assimp): bibliotheque pour charger des modeles 3D dans differents formats.
- [stb](https://github.com/nothings/stb) contient les header *stb_image.h* et *stb_image_write.h* de la bibliothèque stb, afin de lire et écrire des images.
- [tiny_gltf](https://github.com/syoyo/tinygltf): bibliotheque pour charger des modeles 3D au [format glTF](https://github.com/KhronosGroup/glTF), très présent sur le site de partage [https://sketchfab.com/](https://sketchfab.com/).
- [tinyobjloader](https://github.com/syoyo/tinyobjloader): bibliotheque pour charger des modeles 3D au format OBJ. Remplace assimp par defaut sur linux, car plus legère (et le quota d'espace disque à la FAC est faible...)

Mon objectif n'étant pas de vous apprendre à utiliser ces libs, je détaillerais assez peu leurs fonctionnalités dans les TPs. Les liens ci-dessus redirigent vers des documentations assez complètes pour que vous puissiez vous débrouiller :)

A noter que ImGui fournie une fonction ImGui::ShowDemoWindow dont le code montre des exemples d'utilisation d'a peu près tous les widgets de la lib. Le code de cette fonction est dans le fichier *imgui_demo.cpp* du repertoire contenant la lib.

## Lib interne (glmlv)

La bibliothèque interne, dont le code est situé dans le repertoire *lib* du template, est destiné à contenir le code partagé entre les applications. **Les classes et fonctions réutilisables doivent donc y être placées**.

Le template de base contient déjà plusieurs fichiers dans la lib:

- filesystem.hpp *Inclut une bibliothèque de manipulation du système de fichier (soit la lib experimentale de la std, soit la lib de boost si cmake a été appelé avec l'option -DGLMLV_USE_BOOST_FILESYSTEM)*
- gl_debug_output.cpp/hpp *Définit la fonction initGLDebugOutput() permettant d'activer les message de debug d'OpenGL*
- glfw.hpp *Inclut simplement les header de GLFW et glad dans le bon ordre afin d'éviter des erreurs de compil*
- GLFWHandle.hpp *Définit la classe GLFWHandle qui initialise GLFW, ouvre une fenetre, initialise OpenGL avec glad et initialise ImGUI dans son constructeur*
- GLProgram.hpp, GLShader.hpp *Définissent des classes et fonction helpers pour charger, compiler et linker des shaders GLSL*
- Image2DRGBA.hpp *Définit la classe Image2DRGBA et des fonctions de lecture/écriture d'image en surcouche de la lib stb*
- scene_loading.hpp *Encapsule le chargement d'une scene avec Assimp*
- simple_geometry.hpp *Permet la representation de maillages simples et la creation de triangle, cube et sphere*
- ViewController.hpp *Propose une gestion simple FPS d'une caméra*

## Gestion des shaders

> Si vous n'avez pas encore fait la partie Bronze, le terme shader ne doit pas trop vous parler. Revenez donc plus tard !

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
