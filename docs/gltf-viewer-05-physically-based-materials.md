---
id: gltf-viewer-05-physically-based-materials
title: WIP - Physically based materials
---

Work in Progress.

If you arrive to this point but I still have not written this tutorial, here is the roadmap, you should be able to do it by yourself.

Documentation:

- Carefully read the parts "textures, images, samplers" and "materials" of the quick reference image https://raw.githubusercontent.com/KhronosGroup/glTF/master/specification/2.0/figures/gltfOverview-2.0.0b.png. Note that `tinygltf` loads for you the images, so you don't need to read image files yourself.
- Read the parts https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#texture-data and https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#materials of the specification.
- Read appendix B https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#appendix-b-brdf-implementation for more details about the BRDF.

Proposed steps for implementation:

1. Start by trying to adapt our white diffuse model to the material base color. The goal is to replace our constant (\frac{1}{\pi}, \frac{1}{\pi}, \frac{1}{\pi}) with the base color of the material of the object
   1.  You need to add a uBaseColorFactor in your fragment shading and use it for the lighting calculation
   2.  At the moment you draw a mesh in your drawing function, you need to get the material of this mesh
   3.  Then set the base color factor uniform from what is stored in the material
2. Then use textures
   1. You need to create openGL textures for each texture of the glTF
   2. You need to add a sampler2D uBaseColor uniform in your shader and use it for the lighting calculation (multiply what you read from it with the base color factor)
   3. And again in the drawing function, you need to bind the texture from what is stored in the material
3. At this point, you should already have correct colors and textures with just diffuse shading. Now you need to implement the full shading model by following the documentation (specification + appendix). If you do it properly, you should have nice metallic or plastic materials.

And if you want to finish it, you need to implement (these are bonus / possibilities for the project):
- Normal mapping with the normal texture
- Alpha blending to handle transparency (see "alpha coverage" of the specification). This requires depth sorting for correct rendering

If you really want nice and realistic rendering with this kind of material, you would like to implement environment lighting for diffuse and glossy. You can find tutorials here (again, these are bonus / possibilities for the project):
- https://learnopengl.com/PBR/IBL/Diffuse-irradiance
- https://learnopengl.com/PBR/IBL/Specular-IBL