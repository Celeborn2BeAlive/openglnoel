---
id: gltf-viewer-04-directional-lighting
title: Directional Lighting
---

## Introduction to lighting

We will now add a light to our scene and a basic diffuse material model (just to check that our lighting code is working and before implementing the material model of glTF).

The type of light we will implement is the "directional light". A directional light emits from a single direction all points of the scene. It is a coarse approximation of a distant light like the sun would be for the earth. A directional light can indeed be seen as a point light placed infinitely far and emitting an infinite amount of energy.

Directional lights are said to be "non physically based", because they are really far from real world lights. Point lights and spot lights are other kinds of non physically based lights.

Examples of physically based lights in other 3D softwares are area lights (with various shapes), environment lights (often described by aa HDR texture) or mesh lights (meshes that act as emitters). These kind of lights are also not perfectly describing the reality, but they are a bit closer (anyway, nothing really describes reality in computer graphics, and what is reality ?).

## Fragments colors

Our goal is to compute a color for each fragment. A color is physically described by a quantity called *radiance*. Radiance has a precise physical definition, but for us it will be a quantification of the "amount" of light in a given direction for each red, green and blue component, so a color.

The direction of interest for us is the direction of the 3D position of the fragment to the eye of the camera. In our fragment shaders, this direction can be obtained by the position in the view space:

```glsl
vec3 viewDirection = -vViewSpacePosition; // Note that it is unormalized here
```

It works because in view space the position of the eye is (0, 0, 0) (in world space, the view direction would be `cameraEye - worldSpacePosition`).

The point that is being illuminated (the "fragment") is referred as the "shading point" and described by its position, normal and texture coordinates (the inputs of the fragment shader).

## Mathematical model

> The model described here is only valid for a single directional light. For multiple directional lights we just need to accumulate the results. For point lights the model is a bit different but not too much. For physically based lights the model includes integrals over the hemisphere of the shading point.

Let \\( \omega_i \\) be the lighting direction (a normalized vector of \\( \mathbb{R}^3 \\)) and let \\( \omega_o \\) be the view direction ("i" stands for "incoming" and "o" stands for "outgoing"). Let \\( L_i \\) be the radiance emitted by the directional light (a color also).

The radiance \\( L(\omega_o) \\)  in the view direction is then expressed by:

$$
L(\omega_o) = f_r(\omega_i, \omega_o)L_i\cos \theta_{n,\omega_i}
$$

In this expression \\( f_r(\omega_i, \omega_o) \\) is a function called the *BRDF*, short for *bidirectional reflectance distribution function*. In the general case, this function is different for every point in the scene, and basically describes the material of each point. It quantify the ratio of "energy" that is transfered from the incoming direction \\( \omega_i \\) to the outgoing direction \\( \omega_o \\).

For now we will use a super simple BRDF:

$$
f_r(\omega_i, \omega_o) = (\frac{1}{\pi}, \frac{1}{\pi}, \frac{1}{\pi})
$$

This BRDF gives an approximation of a white diffuse material.

The cosine \\( \cos \theta_{n,\omega_i} \\) is the cosine of the angle between then normal of the shading point and the lighting direction. It can be obtained with a dot product between the two vectors, provided they are normalized.

With that, we should be able to implement the rendering of our scene with a directional light and allow the user to change its intensity and direction from the GUI.

## The fragment shader

<span class="todo badge"></span> Create a new shader "diffuse_directional_light.fs.glsl" in the shaders directory of the source code and implement the above mathematical model. The lighting direction \\( \omega_i \\) and lighting intensity \\( L_i \\) should be obtained as uniform variables of the shader. The lighting direction is supposed to be already normalized and expressed in view space, so no need to do complex computations for that (we will do that later in `run()`). You can take example on other fragment shaders in the directory. For \\( \pi \\) you can use 3.14.

> Since you are adding a new source file to the project, you will need to re-run cmake (`cmake .` in the build folder) to take it into account.

<span class="todo badge"></span> In ViewerApplication.hpp, change the default shader "normals.fs.glsl" with your new shader.

If you try it now, you should see a black image.

Solution:
- https://github.com/Celeborn2BeAlive/gltf-viewer-tutorial/commit/03262195840dd0e66e1661188266e8d4c4c3f556

## Send light parameters from the application

We now need to send the light parameters from the application. For that we need to get uniform locations with `glGetUniformLocation` at the begining of `run()` (like other uniforms).

Then in the render loop we need to set our uniforms with `glUniform3f`. For the light direction, we must be careful to muliply it with the view matrix, and normalize it, before sending it to the shader. Otherwise, we will see the light move as we move the camera, and that's not what we want of course. 

Before the render loop:

<span class="todo badge"></span> Get uniforms for light parameters.

<span class="todo badge"></span> Declare and initialize two `glm::vec3` variables `lightDirection` and `lightIntensity`.

During the render loop (in `drawScene` lambda, after getting the view matrix and before the loop over nodes): 

<span class="todo badge"></span> Send `lightIntensity` to the shader and transform to view space and normalize `lightDirection` then pass it to the shader. However, before sending the data, check if the location are not negative (it can happen if we use another shader than our lighting fragment shader).

Solution:
- https://github.com/Celeborn2BeAlive/gltf-viewer-tutorial/commit/9bbad4eacaaf8b2820f32c7e44d14cd851e8a3bc

## Control the light from the GUI

Our light direction is expressed as a vector, but it is not really intuitive to set from a GUI. A better control can be done with two angles (spherical coordinates), to convert to a direction with the following equation:

$$
\omega_i = (\sin \theta \cos \phi, \cos \theta, \sin \theta \sin \phi)
$$

<span class="todo badge"></span> In the GUI, add a new section "Light" (with  `ImGui::CollapsingHeader()`) containing two sliders for \\( \theta \\) (between 0 and \\( \pi \\)) and \\( \phi \\) (between 0 and \\( 2\pi \\)) angles. When they are changed, compute `lightDirection` from them using the above formula.

For the intensity, it is better to give the user the possibility of choosing a color and a scalar intensity, then to multiply both before sending it to the shader.

<span class="todo badge"></span> Add a `ImGui::ColorEdit3` to specify a color and an input float for the intensity factor. If they are changed, update `lightIntensity` with their product.

Solution:
- https://github.com/Celeborn2BeAlive/gltf-viewer-tutorial/commit/ea7420667a899d33d613754fc40404da50d58fae

## Lighting from camera

It can sometimes be useful to emit light from the camera instead of a fixed direction. It is kind of easy from the code: just send the negative view direction `-camera.front()` to the light direction uniform instead of `lightDirection`. Actually, this is even easier in our case because we are doing calculations in view space: just send (0, 0, 1), since in view space the view direction is the negative z axis.

<span class="todo badge"></span> Add a checkbox "light from camera" that control a boolean. If set, then send (0, 0, 1) instead of `lightDirection` in the drawing function for the uniform controlling the light direction.

Solution:
- https://github.com/Celeborn2BeAlive/gltf-viewer-tutorial/commit/59625f2a0cdd7ac22ee82a54ec5a9752c267cbeb