---
id: gold-02-forward-renderer-03-shaders
title: Shaders
---

Créez un sous-dossier *shader* dans le dossier de l'application *foward-renderer* et créez le fichiers *forward.vs.glsl* et *forward.fs.glsl*.

Voir les différents shaders des applications déjà présente pour vous aider.

[Ce TD de l'année précedente](http://igm.univ-mlv.fr/~lnoel/index.php?section=teaching&teaching=opengl&teaching_section=tds&td=td5) explique également la théorie.

## Le Vertex Shader

- Le vertex shader doit prendre en entrée une position (vec3 aPosition), une normale (vec3 aNormal) et des texCoords (vec2 aTexCoords)
- N'oubliez pas de définir les locations des attributs
- Ajouter des variable uniformes *mat4 uModelViewProjMatrix*, *mat4 uModelViewMatrix*, *mat4 uNormalMatrix* correspondant aux matrices standard en 3D
- Ajouter des variables out *vec3 vViewSpacePosition*, *vec3 vViewSpaceNormal*, *vec3 vTexCoords*
- Dans le main du shader, remplir les différentes variables out et la variable *gl_Position* en respectant le pseudo-code suivant et en faisant les convertions de type necessaires:

```
vViewSpacePosition = uModelViewMatrix * aPosition
vViewSpaceNormal = uNormalMatrix * aNormal
vTexCoords = aTexCoords
gl_Position = uModelViewProjMatrix * aPosition
```

## Le Fragment Shader

En attendant d'implémenter un modèle d'illumination, faite en sorte que le fragment shader affiche les normales des objets.