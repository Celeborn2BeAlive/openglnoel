---
id: bronze-06-camera-02-trackball-camera
title: Trackball Camera
---

La caméra TrackBall est très simple: elle ne peut que tourner autour du centre de la scène (ou d'un objet) et s'en rapprocher ou s'en éloigner. C'est le type de caméra souvent utilisée pour visualiser des planète. Elle est également utilisé dans les jeux à la troisième personne: elle reste centrée sur le personnage principal auquel duquel on peut tourner.

Nous utiliserons la molette de la souris pour nous rapprocher ou nous éloigner du centre de la scène. La rotation sera gérée par les mouvements verticaux et horizontaux de la souris lorsque le bouton droit est pressé.

<span class="badge todo"></span>  Créez un nouveau fichier **TrackballCamera.hpp** dans la lib de votre répertoire de TP (repertoire glimac/include/glimac).

<span class="badge todo"></span>  Dans ce fichier créez la classe **TrackballCamera**. Celle ci doit contenir trois variables membres privées: **float m_fDistance**, **float m_fAngleX** et **float m_fAngleY**. Ces 3 variables représentent respectivement: la distance par rapport au centre de la scène, l'angle effectuée par la caméra autour de l'axe x de la scène (rotation vers le haut ou vers le bas) et l'angle effectuée par la caméra autour de l'axe y de la scène (rotation vers la gauche ou vers la droite). Implantez ensuite le constructeur en initialisant les angles à 0 et la distance à 5.

![Trackball Camera](/openglnoel/img/trackball.svg)

<span class="badge todo"></span>  Implantez ensuite les méthodes suivantes:

- **void moveFront(float delta)** permettant d'avancer / reculer la caméra. Lorsque **delta** est positif la caméra doit avancer, sinon elle doit reculer.
- **void rotateLeft(float degrees)** permettant de tourner latéralement autour du centre de vision.
- **void rotateUp(float degrees)** permettant de tourner verticalement autour du centre de vision.
Ces 3 méthodes doivent simplement additionner le paramètre reçu à la bonne variable membre.

<span class="badge todo"></span>  Il faut ensuite coder la méthode **glm::mat4 getViewMatrix()** const qui calcule la ViewMatrix de la caméra en fonction de ses variables membres. Comme je l'ai dit dans l'introduction, déplacer la caméra c'est comme déplacer la scène à l'envers. Vous pouvez donc construire la ViewMatrix à partir d'une multiplication de 3 matrices: une translation faisant reculer la scène (on place ainsi la caméra à la bonne distance), une rotation autour de l'axe X puis une rotation autour de l'axe Y. Attention: il faut convertir les angles en radians.

<span class="badge todo"></span>  Modifiez ensuite le code de l'exercice précédent pour pouvoir tourner autour de la planète en utilisant votre souris. Pour cela vous devez créer une TrackBallCamera et appelez ses méthodes en fonctions des événements souris et clavier (utilisez les fonctions de la classe **SDLWindowManager**). Puisque la caméra est susceptible de changer à chaque tour de boucle, vous devez au début de chaque tour récupérer la ViewMatrix en utilisant la méthode **getViewMatrix()** puis la combiner aux matrices Projection et Model pour construire correctement les 3 matrices (MVMatrix, NormalMatrix et MVPMatrix) avant de les envoyer au shader.