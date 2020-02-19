---
id: gltf-viewer-05-physically-based-materials
title: WIP - Physically based materials
---

## Introduction to glTF material model

As explained in [the previous section on lighting](gltf-viewer-04-directional-lighting.md#mathematical-model), the material at a specific point is described by a bidirectional reflectance distribution function (BRDF). We used a simple diffuse white model, but in the general case the BRDF is different for each point.

It is expressed from material properties such as a diffuse color, or a glossiness factor, and these properties are generally stored in textures when they are not constant on the whole mesh.

The glTF documentation gives us all we need to understand the material model used by the format:
- [The quick reference card](https://raw.githubusercontent.com/KhronosGroup/glTF/master/specification/2.0/figures/gltfOverview-2.0.0b.png) has a "materials" section describing how a material is represented, and a "texture, images, samplers" section describing how textures are represented. We also see in the "meshes" section that each mesh primitive can have a `material` properties containing the index of the material for this primitive.
- [The texture data section](https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#texture-data) of the specification explain in details how textures, images and samplers are stored in a glTF file.
- [The materials section](https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#materials) explain in details how a material is described, how to combine factors with textures, etc.
- [The BRDF Implementation appendix](https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#appendix-b-brdf-implementation) gives equations to implement the BRDF \\( f_r(\omega_i, \omega_o) \\) at a specific point.
- Finally, [the source code for a webGL implementation](https://github.com/KhronosGroup/glTF-Sample-Viewer/) is available. The shaders of this repository expose an implementation of the BRDF, however it can be hard to read for someone not familiar with the code.

In this tutorial I only describe concepts required for our (incomplete) implementation of the material model. For more details you need to read the specification.

Here is our roadmap to implement the model:
- Texture objects creation
- Creation of the shader pbr_directional_light.fs.glsl with handling of diffuse component (GLSL side)
- Send uniform data to OpenGL for handling diffuse component (C++ side)
- Test that everything work at this point
- Finalization of the shader to handle glossy component (GLSL side)
- Send remaining uniform data to OpenGL for the glossy component (C++ side)

## Texture objects creation

### glTF textures

The glTF contains a `textures` array describing all textures that can be referenced by materials.

A texture has the following shape:
```json
{
    "textures": [
        {
            "sampler": 0,
            "source": 2
        }
    ]
}
```
The `source` attribute is the index of an image, the `sampler` attribute is the index of a sampler.

#### Images

Images are stored in an `images` array, with the following shape:
```json
{
    "images": [
        {
            "uri": "duckCM.png"
        },
        {
            "bufferView": 14,
            "mimeType": "image/jpeg" 
        }
    ]
}
```
Here we have two images in the array: one is a file, the second is a jpeg store in a buffer of the glTF file.

If we were doing everything from scratch, we would need to manually load images from disk or buffers. Fortunately, the tinygltf library does that for us so we can directly access the data and send it to the GPU in texture objects. This code examples shows how to fill a texture object using an image from tinygltf:

```cpp
// Assume a texture object has been created and bound to GL_TEXTURE_2D
const auto &texture = model.textures[i]; // get i-th texture
assert(texture.source >= 0); // ensure a source image is present
const auto &image = model.images[texture.source]; // get the image

// fill the texture object with the data from the image
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
        GL_RGBA, image.pixel_type, image.image.data());
```

#### Samplers

A sampler describes how a texture must be read by OpenGL: what filters to use, what wrapping mode to use. Similar to all glTF objects, they are stored in a `samplers` array, with the following shape:
```json
{
    "samplers": [
        {
            "magFilter": 9729,
            "minFilter": 9987,
            "wrapS": 10497,
            "wrapT": 10497
        }
    ]
}
```
The constant number here correspond to OpenGL constants such as `GL_LINEAR`, `NEAREST_MIPMAP_NEAREST`, `GL_REPEAT`, etc. It means we can directly use the values to setup texture sampling with the OpenGL API:

```cpp
// Assume a texture object has been created and bound to GL_TEXTURE_2D
const auto &texture = model.textures[i]; // get i-th texture
const auto &sampler =
        texture.sampler >= 0 ? model.samplers[texture.sampler] : defaultSampler;
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
   sampler.minFilter != -1 ? sampler.minFilter : GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
   sampler.magFilter != -1 ? sampler.magFilter : GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sampler.wrapS);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sampler.wrapT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, sampler.wrapR);
```
When a filter is undefined, we use `GL_LINEAR` as default filter. And when no sampler is defined for the texture (in that case we have `texture.sampler == -1`), we use a default sampler defined as:

```cpp
tinygltf::Sampler defaultSampler;
defaultSampler.minFilter = GL_LINEAR;
defaultSampler.magFilter = GL_LINEAR;
defaultSampler.wrapS = GL_REPEAT;
defaultSampler.wrapT = GL_REPEAT;
defaultSampler.wrapR = GL_REPEAT;
```

Some samplers use mipmapping for their minification filter. In that case, the specification tells us we need to have mipmaps computed for the texture. OpenGL can compute them for us:
```cpp
if (sampler.minFilter == GL_NEAREST_MIPMAP_NEAREST ||
   sampler.minFilter == GL_NEAREST_MIPMAP_LINEAR ||
   sampler.minFilter == GL_LINEAR_MIPMAP_NEAREST ||
   sampler.minFilter == GL_LINEAR_MIPMAP_LINEAR) {
   glGenerateMipmap(GL_TEXTURE_2D);
}
```

## Diffuse component in GLSL

### Reading textures in GLSL

In GLSL, it is quite easy to declare and use a texture. Textures are declared as samplers in uniform variables, and we read them with the texture function.

For example a simple shader using texture could be like that:
```glsl
#version 330

in vec3 vTexCoords;

uniform sampler2D uTextureSky;
uniform sampler2D uTextureCloud;

out vec3 fColor;

void main()
{
   vec4 skyColor = texture(uTextureSky, vTexCoords);
   vec4 cloudColor = texture(uTextureCloud, vTexCoords);

   fColor = skyColor.rgb + cloudColor.rgb; // we don't use the alpha component in this simple shader
}
```
Here we have two texture and we add the colors we read from them.

However, a subtility might occur when we manipulate material textures. This is the case for the base color texture of glTF (but not the metal roughness texture that we'll use later).

The base color texture has RGB values encoded in sRGB color space. It basically means that these values are not real lighting intensities as you would compute them in a render engine (or lighting simulation), but have been modified to be displayed on a computer screen. This transform is necessary if we want to be able to see the same image on a computer screen as we would see in reality, on a printing for example.

You can find more information [on wikipedia](https://en.wikipedia.org/wiki/SRGB) about this transform and why it exists. Another good resource about color is [The Hitchiker's Guide to Digital Colour](https://hg2dc.com/).

What does it mean for us ? It means we have to invert the sRGB transform 

---

Work in Progress.

If you arrive to this point but I still have not written this tutorial, here is the roadmap, you should be able to do it by yourself.

Documentation:

- Carefully read the parts "textures, images, samplers" and "materials" of the quick reference image https://raw.githubusercontent.com/KhronosGroup/glTF/master/specification/2.0/figures/gltfOverview-2.0.0b.png. Note that `tinygltf` loads for you the images, so you don't need to read image files yourself.
- Read the parts https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#texture-data and https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#materials of the specification.
- Read appendix B https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#appendix-b-brdf-implementation for more details about the BRDF.

Proposed steps for implementation:

1. Start by trying to adapt our white diffuse model to the material base color. The goal is to replace our constant \\( (\frac{1}{\pi}, \frac{1}{\pi}, \frac{1}{\pi}) \\) with the base color of the material of the object
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