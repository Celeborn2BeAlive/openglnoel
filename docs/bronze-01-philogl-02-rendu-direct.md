---
id: bronze-01-philogl-02-rendu-direct
title: Le Rendu Direct
---

## Qu'est ce que le rendu direct

Le rendu direct c'est ce que vous avez pratiqué en Synthèse d'Image 1: utiliser les fonctions glBegin, glEnd, glVertex, glColor, etc. dans la boucle principale pour faire votre rendu. Voici un exemple qui dessine un triangle coloré:

```cpp
// OpenGL 2 et rendu direct:
int main() {
    initWindowSystem(); // SDL, GLFW, SFML, etc.
    
    bool loop = true;
    while(loop) {
        glBegin(GL_TRIANGLES);
            glColor3ub(255, 0, 0);
            glVertex2f(-0.5, -0.5);
            glColor3ub(0, 255, 0);
            glVertex2f(0.5, -0.5);
            glColor3ub(0, 0, 255);
            glVertex2f(0., 0.5);
        glEnd();
        
        SDL_GL_SwapBuffers();
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) {
                loop = false;
            }
        }
    }
    
    exitWindowSystem();
    return 0;
}
```

Le résultat:

![Triangle](/openglnoel/img/triangle.png)


## Le problème du rendu direct

Le code précédent cache un gros problème de performance: à chaque tour de boucle l'ensemble des données est renvoyé à la carte graphique (tous les nombres là dans les appels de fonction). De plus appeler autant de fonctions est couteux en temps. Quand il s'agit de dessiner un seul triangle ça va, vous devriez faire tourner le code en temps réel. Par compte s'il faut en dessiner un million, ça va commencer à ramer.

Mettez vous à la place des développeurs OpenGL et essayez de trouver une manière d'optimiser cela. Le but est d'avoir le moins de chose possible à faire dans la boucle de rendu afin de minimiser le temps passé dans cette dernière et ainsi augmenter le nombre d'images calculées par seconde.

![Direct Rendering](/openglnoel/img/direct_rendering.png)

## Le rendu par buffer

Une solution simple est de construire à l'initialition des buffers contenant nos données. Ces buffers sont stockés directement sur la carte graphique et identifiés dans le code CPU par un entier. Il suffit ensuite durant la boucle de rendu de dire à OpenGL "hé ! dessine les données de ce buffer !".

On minimise ainsi les transferts de données entre CPU et GPU or c'est ce qui coute le plus cher.

![Buffer Rendering](/openglnoel/img/vbo_rendering.png)

Voici un exemple de code pour dessiner un triangle... blanc (pour avoir de la couleur c'est plus compliqué). Inutile de lire dans le détail pour l'instant, l'objectif est plutôt d'essayer de comprendre la différence avec le code OpenGL 2.

```cpp
int main() {
    initWindowSystem(); // SDL, GLFW, SFML, etc.
    
    // Initialisation de GLEW; nous verrons dans le prochain TP à quoi cela sert.
    GLint error;
    if(GLEW_OK != (error = glewInit())) {
        std::cerr << "Impossible d'initialiser Glew" << std::endl;
        return 1;
    }
    
    // Création d'un Vertex Buffer Object et d'un Vertex Array Object
    GLuint vbo, vao;
    
    // Allocation d'un Vertex Buffer Object:
    glGenBuffers(1, &vbo);

    // "vbo" devient le VBO courant sur la cible GL_ARRAY_BUFFER:
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
        // Tableau des attributs des sommets
        GLfloat vertices[] = {
            -0.5, -0.5, // Premier vertex
            0.5, -0.5, // Deuxième vertex
            0., 0.5, // Troisème vertex
        };
        
        // Stockage des données du tableau vertices dans le vbo placé sur GL_ARRAY_BUFFER (c'est à dire "vbo" ici) :
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Plus de VBO sur la cible GL_ARRAY_BUFFER:
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // Allocation d'un Vertex Array Objet:
    glGenVertexArrays(1, &vao);
    
    // "vao" devient le VAO courant:
    glBindVertexArray(vao);
    
        // "vbo" devient le VBO courant sur la cible GL_ARRAY_BUFFER:
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        
        // Activation de l'attribut de vertex 0; nous l'interpretons comme la position
        glEnableVertexAttribArray(0 /* Incide attribut*/);
        // On spécifie le type de donnée de l'attribut position ainsi que la manière dont il est stocké dans le VBO
        glVertexAttribPointer(
            0 /* Indice attribut */, 
            2 /* Nombre de composantes */, 
            GL_FLOAT /* Type d'une composante */, 
            GL_FALSE /* Pas de normalisation */, 
            2 * sizeof(GLfloat) /* Taille en octet d'un vertex complet entre chaque attribut position */, 
            0 /* OpenGL doit utiliser le VBO attaché à GL_ARRAY_BUFFER et commencer à l'offset 0 */);
        
    // Plus de VAO courant:
    glBindVertexArray(0);
    
    bool loop = true;
    while(loop) {
        // "vao" devient le VAO courant, OpenGL l'utilisera lors du dessin pour avoir des informations sur les sommets
        glBindVertexArray(vao);
        
        // OpenGL doit dessiner des triangles en utilisant 3 sommets (donc un seul triangle)
        glDrawArrays(GL_TRIANGLES, 0 /* Pas d'offset au début du VBO */, 3);
        
        // Plus de VAO courant:
        glBindVertexArray(0);
        
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) {
                loop = false;
            }
        }

        SDL_GL_SwapBuffers();
    }
    
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    
    exitWindowSystem();
    return 0;
}
```

C'est déjà plus long, et on a même pas la couleur >.< ! On remarque que le code de rendu ne fait maintenant plus que 3 lignes. Toutes les données sont placées dans un Vertex Buffer Object avant la boucle principale. Un Vertex Array Object stocke le format des données qu'on envoit à OpenGL (3 positions composée chacune de 2 flottants).