---
id: silver-04-lighting-02-directional-light
title: Directional Light
---

Une lumière directionnelle éclaire depuis l'infini dans une unique direction tous les objets de la scène. On peut utiliser ce type de lumière pour simuler des sources très lontaines comme le soleil par exemple.

Ce type de lumière sera donc définit par deux propriétés: sa direction incidente (vecteur 3D) et son intensité (couleur).

<span class="badge todo"></span>  Créez un nouveau fragment shader sur le modèle de celui utilisé pour les textures (même variables "in", supprimez la texture). Appelez le **directionallight.fs.glsl**.

<span class="badge todo"></span>  Ajoutez les variables uniformes **vec3 uKd**, **vec3 uKs** et **float uShininess** qui nous serviront à représenter le matériau de l'objet en cours de dessin.

<span class="badge todo"></span>  Ajoutez les variables uniformes **vec3 uLightDir_vs** et **vec3 uLightIntensity** qui nous serviront à passer les informations sur la lumière au shader. Notez le suffise **_vs** sur la direction: cela indique que nous allons travailler dans le view space; il faudra donc multiplier la direction de la lumière par la View Matrix avant de l'envoyer au shader.

<span class="badge todo"></span>  Toujours dans le shader, écrivez la fonction **vec3 blinnPhong()** qui calcule et renvoit la couleur du fragment en fonction des paramètres de matériau et d'éclairage. Il faut pour cela implanter la formule vue précédemment. wi correspond simplement à **uLightDir_vs** normalisé. \\(L_i\\) correspond à **uLightIntensity**. Pour calculer \\(halfVector\\) il faut d'abord calculer \\(\omega_o\\) qui est le vecteur pointant vers la caméra. Or l'avantage d'avoir les positions dans le View Space est que la position de la caméra est à l'origine du repère. Par conséquent \\(\omega_o\\) est simplement l'opposé de la position du fragment normalisée (**normalize(-vPosition_vs)**). N'oubliez pas d'utiliser la fonction **dot** pour les produits scalaire et pow pour l'élévation à la puissance.

<span class="badge todo"></span>  Dans le main du shader, utilisez votre fonction **blinnPhong** pour calculer la couleur finale **fFragColor**.

Il faut à présent écrire le code C++ correspondant.

<span class="badge todo"></span>  Reprenez le code des planètes (avec caméra) en supprimant le chargement et le binding des textures.

<span class="badge todo"></span>  Chargez votre nouveau shader.

<span class="badge todo"></span>  Récupérez les locations des nouvelles variables uniformes (fonction **glGetUniformLocation**)

<span class="badge todo"></span>  Dans la boucle de rendu, envoyez les valeurs des variables uniformes (rappel: en utilisant les fonction de la forme **glUniformXf**). Pour la direction de la lumière, utilisez \\((1,1,1)\\) mais n'oubliez pas de multiplier par la View Matrix avant d'envoyer au shader. Pour les coefficients de matériaux essayez différentes valeurs.

<span class="badge todo"></span>  Faites en sorte que chaque sphère soit dessiné avec un matériau aléatoire.

<span class="badge todo"></span>  Faites en sorte que la lumière tourne autour de la scène (pour cela appliquez une rotation dépendant du temps au vectuer \\((1,1,1)\\) avant de multiplier la View Matrix).