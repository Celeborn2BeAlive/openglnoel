---
id: silver-02-forward-renderer-05-lighting
title: Lighting
---

## Fragment Shader

Implémenter un modèle d'illumination diffuse dans le fragment shader *forward-renderer.fs.glsl*

- Ajouter des variables uniform *vec3 uDirectionalLightDir* et *vec3 uDirectionalLightIntensity* destinés à stocker les paramètres d'une lumière directionnelle (en espace view)
- Ajouter des variables uniform *vec3 uPointLightPosition* et *vec3 uPointLightIntensity* destinés à stocker les paramètres d'une lumière ponctuelle (en espace view)
- Ajouter une variable uniform *vec3 uKd* destiné à stocker la couleur diffuse de l'objet en cours de rendu
- Dans le main, utiliser ces variables ainsi que les attributs du fragment pour calculer la couleur du fragment:

```glsl
float distToPointLight = length(uPointLightPosition - vViewSpacePosition);
vec3 dirToPointLight = (uPointLightPosition - vViewSpacePosition) / distToPointLight;
fColor = uKd * (uDirectionalLightIntensity * max(0.0, dot(vViewSpaceNormal, uDirectionalLightDir)) + uPointLightIntensity * max(0.0, dot(vViewSpaceNormal, dirToPointLight)) / (distToPointLight * distToPointLight))
```

## Application

A l'initialisation:

- Récupérer les locations des nouvelles uniform

Au rendu:

- Envoyer les différente uniformes avant le rendu de chaque objet (les lights doivent être partagé)
- Faire en sortie de pouvoir régler les paramètres des lights et la couleur diffuse des objets depuis la GUI

Fonctions GL à utiliser:

- glUniform3f