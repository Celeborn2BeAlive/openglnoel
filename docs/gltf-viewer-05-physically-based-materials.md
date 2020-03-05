---
id: gltf-viewer-05-physically-based-materials
title: Physically based materials
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

### OpenGL textures

In OpenGL, a texture is represented by a texture object, composed by an image and sampling parameters. Like other OpenGL objects, a texture needs to be generated and bound in order to be able to act on it (fill it with image, set sampling parameters).
The following code shows how to create a texture object and fill it with a 1920x1080 image of RGB pixels:

```cpp
std::vector<float> pixels(1920 * 1080 * 3, 0); // "Useless" black image of RGB (3 components) float values

GLuint texObject;
// Generate the texture object
glGenTextures(1, &texObject);

glBindTexture(GL_TEXTURE_2D, texObject); // Bind to target GL_TEXTURE_2D
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0,
        GL_RGB, GL_FLOAT, pixels.data()); // Set image data
// Set sampling parameters
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// Set wrapping parameters
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

glBindTexture(GL_TEXTURE_2D, 0);
```
In the call `glTexImage2D` we specify two formats:
- The first one (GL_RGBA in this call) is the format of the texture on the GPU. Here we put GL_RGBA but we only send RGB values: OpenGL will automatically fill the last component with 0 for each pixel. This argument is quite free, we can choose what we want that fills our needs.
- The second format (GL_RGB and GL_FLOAT) specifies what format we give to OpenGL. Here our vector `pixels` is filled with floats, so we put GL_FLOAT, and we have 3 components per pixel, so GL_RGB. If our vector was filled with unsigned chars instead of floats, we would put GL_UNSIGNED_BYTE instead of GL_FLOAT. If we had only one component per pixel in our vector, we would put GL_RED instead of GL_RGB. So these two arguments are strongly dependent on our data.

So we know how to create a texture object, let's see how to do it for a texture from a glTF file.

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

If we were doing everything from scratch, we would need to manually load images from disk or buffers. Fortunately, the tinygltf library does that for us so we can directly access the data and send it to the GPU in texture objects. This code example shows how to fill a texture object using an image from tinygltf:

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

### The practice

To manipulate our textures from the input glTF file, we need a few things:

<span class="todo badge"></span> In the `ViewerApplication` class, implement a method with prototype: `std::vector<GLuint> ViewerApplication::createTextureObjects(const tinygltf::Model &model) const;`. This method should compute a vector of texture objects. Each texture object is filled with an image and sampling parameters from the corresponding texture of the glTF file. This step basically consists of picking code examples from this section and putting them in a loop in order to initialize each texture object.

<span class="todo badge"></span> Add a call to your method in the `run()` method, in the initialization step, before the drawing loop (for example, before the call to `createBufferObjects`). Store the result in a vector `textureObjects`.

<span class="todo badge"></span> After the call, create a single texture object with a variable `GLuint whiteTexture` to reference it. Fill it with a single white RGBA pixel (`float white[] = {1, 1, 1, 1};`) and set sampling parameters to `GL_LINEAR` and wrapping parameters to `GL_REPEAT`. This texture will be used for the base color of objects that have no materials (as specified in the glTF specification, if no base color texture is present, we should use white).

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

What does it mean for us ? It means we have to invert the sRGB transform to correctly read the base color texture before doing any lighting computation. Now similarly, to correctly display a color obtained from a lighting computation on a computer screen, we need to apply a transform that gives colors in sRGB space. This is commonly referred as gamma correction (or more generally "tone mapping"). Here is an example of a simple diffuse shader that uses a base color texture to get the diffuse color:

```glsl
#version 330

in vec3 vViewSpaceNormal;
in vec2 vTexCoords;

uniform vec3 uLightDirection;
uniform vec3 uLightIntensity;

uniform sampler2D uBaseColorTexture;

out vec3 fColor;

// Constants
const float GAMMA = 2.2;
const float INV_GAMMA = 1. / GAMMA;
const float M_PI = 3.141592653589793;
const float M_1_PI = 1.0 / M_PI;

// We need some simple tone mapping functions
// Basic gamma = 2.2 implementation
// Stolen here: https://github.com/KhronosGroup/glTF-Sample-Viewer/blob/master/src/shaders/tonemapping.glsl

// linear to sRGB approximation
// see http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
vec3 LINEARtoSRGB(vec3 color)
{
  return pow(color, vec3(INV_GAMMA));
}

// sRGB to linear approximation
// see http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
vec4 SRGBtoLINEAR(vec4 srgbIn)
{
  return vec4(pow(srgbIn.xyz, vec3(GAMMA)), srgbIn.w);
}

void main()
{
  vec3 N = normalize(vViewSpaceNormal);
  vec3 L = uLightDirection;

  vec4 baseColorFromTexture = SRGBtoLINEAR(texture(uBaseColorTexture, vTexCoords));
  float NdotL = clamp(dot(N, L), 0, 1);
  vec3 diffuse = baseColorFromTexture.rgb * M_1_PI;

  fColor = LINEARtoSRGB(diffuse * uLightIntensity * NdotL);
}
```
You should see some similarities with the "white diffuse" shader of the previous section. The main differences are:
- instead of white we use a color from the texture
- we apply tone mapping functions to compute lighting in a linear RGB color space, but to output values in sRGB color space (for the screen display).

<span class="todo badge"></span> Copy this shader in a file called `pbr_directional_light.fs.glsl` in the shaders folder.

<span class="todo badge"></span> In `ViewerApplication.hpp`, replace the default fragment shader with this new shader.

<span class="todo badge"></span> Try your application, you should have a black image at this point.

### Binding textures for rendering

As any uniform variable, there is a bit of work to do in the C++ side of the code to send/bind values before drawing each object.

The first step is to get the uniform location of `uBaseColorTexture`.

<span class="todo badge"></span> In the `run()` method, get the uniform location of `uBaseColorTexture` with `glGetUniformLocation` (put it in the code where other uniform locations are obtained).

Then just before drawing a primitive, we need to bind the base color texture and tell OpenGL where it is bound.

By "where it is bound", we mean "on which texture unit" it is bound. But what are texture units ?

In order to use multiple textures at the same time in a shader (like uTextureSky and uTextureCloud of first example of shader), OpenGL offers us multiple texture units, represented by an index. The following C++ example shows an example of binding two textures that could be used for the first shader example:

```cpp
// Assume we have:
// - uTextureSkyLocation, the uniform location of uTextureSky
// - uTextureCloudLocation, the uniform location of uTextureCloud
// - skyTexObject, a texture object containing a texture for the sky
// - cloudTexObject, a texture object containing a texture for the clouds

// Bind skyTexObject to target GL_TEXTURE_2D of texture unit 0
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, skyTexObject);
// By setting the uniform to 0, we tell OpenGL the texture is bound on tex unit 0:
glUniform1i(uTextureSkyLocation, 0);

// Bind cloudTexObject to target GL_TEXTURE_2D of texture unit 1
glActiveTexture(GL_TEXTURE1);
glBindTexture(GL_TEXTURE_2D, cloudTexObject);
// By setting the uniform to 1, we tell OpenGL the texture is bound on tex unit 1:
glUniform1i(uTextureCloudLocation, 1);
```
After that we could draw some object (a sky dome ?) that would represent a sky with clouds.

A thing to keep is mind is that, like most calls of OpenGL, everything acts on some global state. So when we do `glActiveTexture(GL_TEXTURE1);`, then any subsequent calls to texture functions (like `glBindTexture`) will act on the texture unit 1 and no more on the texture unit 0 (which is the default one).
When we implemented the function `createTextureObjects` we did a bunch of texture related function calls. These calls act on the currently active texture unit. In our case it should be texture unit 0 since we did not call `glActiveTexture` before. But in another context, a big application with a lot of code, it might become an issue to just blindly call texture functions of OpenGL without ensuring we have the correct texture unit activated.

So in our case we will use texture units but just to bind multiple textures. For now we only have base color texture, but later we will have the metallic roughness texture.

In order to have a more or less clean implementation, we will implement the texture binding in a specific lambda function `bindMaterial(int materialIdx)`. For now this function will just bind the base color texture, but it will quickly grow so having the code in a specific function is better. Some other code of the drawing function could also be extracted in dedicated functions, to make things more clear, but it is up to you to do some refactor.

<span class="todo badge"></span> Just before the definition of the `drawScene` lambda function, add the following lambda function:
```cpp
const auto bindMaterial = [&](const auto materialIndex) {
   // Material binding
};
```

<span class="todo badge"></span> In the `drawScene` lambda function, just before drawing a specific primitive (before binding its VAO), add a call to `bindMaterial` with the material index of the primitive as argument.

<span class="todo badge"></span> In `bindMaterial`, implement two cases:
- if the material index is >= 0, and if this material has a base color texture, bind the base color texture of the material to the texture unit 0 and set the associated uniform to 0.
- otherwise, bind the white texture to the texture unit 0 and set the base color texture uniform to 0.

As previously, you need a bit of indirections to access the texture index of the base color texture:
```cpp
// only valid is materialIndex >= 0
const auto &material = model.materials[materialIndex];
const auto &pbrMetallicRoughness = material.pbrMetallicRoughness;
// only valid if pbrMetallicRoughness.baseColorTexture.index >= 0:
const auto &texture = model.textures[pbrMetallicRoughness.baseColorTexture.index];
```

<span class="todo badge"></span> Test your application, at this point you should see textures of your scene.

### Using the base color factor

So far we only used the texture of the base color. But the glTF specification specify that we can also have a RGBA factor for the base color. This factor needs to be muliplied by the value read from the texture in order to obtain the full base color.

This factor is stored in `pbrMetallicRoughness.baseColorFactor` (double array that should contain 4 values). The following code shows how to set a vec4 uniform with this factor:
```cpp
glUniform4f(uBaseColorFactorLocation,
   (float)pbrMetallicRoughness.baseColorFactor[0],
   (float)pbrMetallicRoughness.baseColorFactor[1],
   (float)pbrMetallicRoughness.baseColorFactor[2],
   (float)pbrMetallicRoughness.baseColorFactor[3]);
```

<span class="todo badge"></span> In the shader, add a uniform `uniform vec4 uBaseColorFactor;` and use it in the main function to multiply the `baseColorFromTexture` variable and store the result in a variable `baseColor`. Use this new variable for the diffuse computation.

<span class="todo badge"></span> In the `run()` function of `ViewerApplication`, get the uniform location for `uBaseColorFactor`.

<span class="todo badge"></span> In `bindMaterial`, set the uniform with `glUniform4f` as shown previously. If their is no material, set it to white (1, 1, 1, 1).

## Diffuse + Glossy material model in GLSL

We now need to complete the shader with the code to handle the glossy component. The diffuse component also have to be modified to match the equations of the model.

As said previously, the goal is to implement the model described here: https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#appendix-b-brdf-implementation, so keep it open and read the appendix carefully.

<span class="todo badge"></span> Add uniform variables `float uMetallicFactor`, `float uRougnessFactor` and `sampler2D uMetallicRoughnessTexture` to the shader.

Similar to base color, we will need to multiply the factor by the value read in the texture.

The uMetallicRoughnessTexture contains the metallic value in the blue component, and the rougness value in the green component (source: https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md#pbrmetallicroughnessmetallicroughnesstexture).
Also you don't need to call `SRGBtoLINEAR()` when you read this texture because the values are not really colors, so they are already in a linear space and not sRGB space.

<span class="todo badge"></span> Implement the full model from the equations in the `main()` function of the shader.

Here you really just need to "translate" the mathematical equations to GLSL code. Here are a few tips/hints:
- The `lerp(A, B, x)` function mentionned in the specification can be computed in GLSL with `mix(A, B, x)`.
- You already have N and L variables which are the normal and the light direction. You can obtain the view direction V as `vec3 V = normalize(-vViewSpacePosition);` (works because position in view space is the same as direction), and H as `vec3 H = normalize(L + V);`.
- Keep the same variable names are symbols in the equations, it helps to debug. So \\( f_{diffuse} \\) should be `f_diffuse` in your code, for example.
- clamp all dot products between 0 and 1, like it is already done for `NdotL` in the current implementation of the shader
- There is two equations for \\( f_{specular} \\) in the specification. Only the second one is important for us (\\( F * Vis * D \\))
- The same is true for \\( Vis \\), there is two equation, only the second one is important (the long one, witch square roots in the denominator)
- Check your denominator before dividing to compute Vis: if zero or negative, you should put Vis = 0.
- At the end you should compute \\( f = f_{diffuse} + f_{specular} \\). One computed, you obtain the final color of the fragment as `LINEARtoSRGB((f_diffuse + f_specular) * uLightIntensity * NdotL)`.
- Finally, at some point you need to compute the schlick fresnel F that contains a power of 5. To compute a power of 5, don't use `pow()` function (expensive), instead do something like this:
```glsl
// You need to compute baseShlickFactor first
float shlickFactor = baseShlickFactor * baseShlickFactor; // power 2
shlickFactor *= shlickFactor; // power 4
shlickFactor *= baseShlickFactor; // power 5
```

Good luck, and have fun !

## Setting uniforms and binding metallic-roughness texture

The last thing to do is to set uniforms and bind the metallic rougness texture. Everything should be the same as we did before: getting uniform locations, reading data from the texture object of the glTF file, setting uniform and binding the texture. We already bound the base color texture to the texture unit 0, so we need to bind this new one on texture unit 1. If there is no material or texture, the specular component should be black: for that it is enough to bind 0 (the same as unbinding).

<span class="todo badge"></span> Finish the C++ side by doing what needs to be done, and test you application.

## Emissive part

The last part of the material model to implement is the emission component.

Like other material properties, it is stored in a texture and factor, so nothing new.

The emission component just need to be added to the fragment color.

<span class="todo badge"></span> In the shader, add a texture uniform for the emissive texture and a vec3 uniform for the emissive factor. Use them to compute the emissive component and add it to the fragment color.

<span class="todo badge"></span> In the C++ code, apply the modifications needed to make it work (get uniform locations, bind textures, set uniforms)

## What should be done next

At this point we have what is called a "PBR material model", PBR standing for "physically based rendering". If you search information about this on internet, you should find a lot of resources.

However, the material model of glTF is a bit more complex and allows to add more details to the appearance of an object:

- An occlusion map, which is just a factor to apply to the whole lighting equation, quite easy too
- A normal map: this one is harder, but allows to get a really realistic look by simulating some kind of microgeometry (bonus / possibility for the project)
- Alpha blending to handle transparency (see "alpha coverage" of the specification). This requires depth sorting for correct rendering (bonus / possibility for the project)

Also all we did for lighting is a directional light. This is quite restrictive and very unrealistic. If you really want nice and realistic rendering with PBR materials, you should definitely implement environment lighting for diffuse and glossy components. You can find tutorials here (these are bonus / possibilities for the project):
- https://learnopengl.com/PBR/IBL/Diffuse-irradiance
- https://learnopengl.com/PBR/IBL/Specular-IBL