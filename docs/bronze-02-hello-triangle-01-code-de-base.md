---
id: bronze-02-hello-triangle-01-code-de-base
title: Le code de base
---

> <span class="badge warning"></span> La partie Bronze n'a pas encore été adaptée au nouveau template disponible sur Github.

Commençons par jeter un oeil au code de base du fichier TP_template/SDLtemplate.cpp. Ce code est en C++, que vous ne connaissez peut-être pas encore. Il faudra vous y faire car vous allez en bouffer !

```cpp
#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>

using namespace glimac;

int main(int argc, char** argv) {
    // Initialize SDL and open a window
    SDLWindowManager windowManager(800, 600, "GLImac");

    // Initialize glew for OpenGL3+ support
    GLenum glewInitError = glewInit();
    if(GLEW_OK != glewInitError) {
        std::cerr << glewGetErrorString(glewInitError) << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW Version : " << glewGetString(GLEW_VERSION) << std::endl;

    /*********************************
     * HERE SHOULD COME THE INITIALIZATION CODE
     *********************************/

    // Application loop:
    bool done = false;
    while(!done) {
        // Event loop:
        SDL_Event e;
        while(windowManager.pollEvent(e)) {
            if(e.type == SDL_QUIT) {
                done = true; // Leave the loop after this iteration
            }
        }

        /*********************************
         * HERE SHOULD COME THE RENDERING CODE
         *********************************/

        // Update the display
        windowManager.swapBuffers();
    }

    return EXIT_SUCCESS;
}
```

Comme vous pouvez le constater le code est assez court. La classe SDLWindowManager se charge de gérer ce qui concerne la SDL: ouverture de fenêtre, récupération d'évenements clavier-souris. Ainsi la ligne:
```cpp
SDLWindowManager windowManager(800, 600, "GLImac");
```

va ouvrir une fenêtre de résolution 800x600 dont le titre est "GLImac". La variable windowManager sera utilisée par la suite pour gérer les évenements.

La portion de code suivante:

```cpp
GLenum glewInitError = glewInit();
if(GLEW_OK != glewInitError) {
    std::cerr << glewGetErrorString(glewInitError) << std::endl;
    return EXIT_FAILURE;
}
```

initialise la bibliothèque GLEW. Cette bibliothèque charge l'ensemble des fonctions OpenGL3. Sans elle on peut considerer que l'on a accès qu'aux fonctions OpenGL2.

On affiche ensuite des informations sur OpenGL afin de vérifier que l'on a la bonne version:

```cpp
std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
std::cout << "GLEW Version : " << glewGetString(GLEW_VERSION) << std::endl;
```

Pour les TPs, vous devrez placer le code d'initialisation à cet endroit:

```cpp
/*********************************
 * HERE SHOULD COME THE INITIALIZATION CODE
 *********************************/
```

Ensuite vient le code de la boucle d'application.

```cpp
bool done = false;
while(!done) {
```

Cette boucle s'executera tant que la variable done vaut false.

Au sein de la boucle d'application, on trouve la boucle de traitement d'évenements:

```cpp
SDL_Event e;
while(windowManager.pollEvent(e)) {
    if(e.type == SDL_QUIT) {
        done = true; // Leave the loop after this iteration
    }
}
```

Lorsqu'on évenement survient (clic souris, touche clavier, déplacement souris, etc.), la SDL l'enregistre dans une pile appelée pile d'évenement. La methode windowManager.pollEvent(e) permet de dépiler le dernier évenement reçu afin de le traiter dans la boucle. Tant qu'il reste des évenements, la méthode renvoit true et la boucle se relance.
Actuellement le seul évenement traité est SDL_QUIT, qui est déclenché lorsque l'utilisateur ferme la fenêtre. Dans ce cas on fixe done à true, ce qui aura pour effet de quitter la boucle d'application et terminer le programme. Par la suite vous aurez à rajouter d'autres traitement d'évenements dans cette boucle.

Le code de rendu 2D/3D que vous aurez à écrire viendra après la boucle d'évenement, à la place de :

```cpp
/*********************************
* HERE SHOULD COME THE RENDERING CODE
*********************************/
```

Enfin, l'appel à la méthode windowManager.swapBuffers(); met à jour l'affichage de la fenêtre en pratiquant le double buffering (que vous verrez en cours).

Ce code est la base de toute application multimédia interactive. Il est donc important de bien comprendre sa structure.