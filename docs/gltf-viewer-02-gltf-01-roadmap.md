---
id: gltf-viewer-02-gltf-01-roadmap
title: Roadmap
---

# Roadmap

To implement our viewer, we will follow this roadmap:
- Load gltf file passed on command line with the library tiny_gltf
- Use loaded data to fill buffers on the GPU
- Load shaders passed on the command line
- Build projection and view matrix using the maths library glm
- Setup GL state for rendering
- Implement the render loop:
  - Render the scene
  - Render a simple user interface with the library ImGUI
- Implement the possibility of rendering the scene in an output file (for testing purpose)

The step "Render the scene" will use the data on the GPU (vertex array objects) and traverse the scene graph to draw all objects.

Most of our code will be implemented in a class named ViewerApplication.

The main() function will only parse command line arguments (using the library args), pass these arguments to ViewerApplication constructor, and then call run() on the ViewerApplication object.

To avoid spending too much time with the parsing of command line arguments, I already implemented the main() function. You will only fill ViewerApplication.

The constructor will not do much, just initialize some class members from the arguments.

All the functional code will be implemented in the run() method. This method is supposed to return 0 if the program had no error, otherwise it returns a value different from 0.

The run() method will be responsible of all the points mentionned earlier. We will try to separate the code into several functions to keep it clean, but we also want to avoid over-enginering it to also keep it clear and easy to read.

# Loading the glTF file

The path to the glTF file is stored in the member variable `m_gltfFilePath`.

A glTF file is just a json file with references to binary files to store geometry data and images to store textures.

The tinygltf library is included in the project and can parse the json for us, as well as loading data from referenced file.

<span class="todo badge"></span> Implement a method `bool loadGltfFile(tinygltf::Model & model);` that should use `m_gltfFilePath.string()` to load the correct file into the model structure. The method return value should correspond to what gltf returns. If there is errors or warning, print them.

For that, use the readme of the tinygltf repository https://github.com/syoyo/tinygltf

<span class="todo badge"></span> In the run() method, define a `tinygltf::Model` instance and call your method on it.

# Understanding glTF data

The glTF format is precisely defined on its dedicated repository: https://github.com/KhronosGroup/glTF

No other resource is needed to understand and use the format.

The first file to look at is a quick reference card: https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/figures/gltfOverview-2.0.0b.png (open it in new tab or download it to zoom on it).

It gives a large overview of how a scene is described by a glTF file is described and is most often enough.

When more precise explanation are required, as well as exact type information and knowing if a field is optional or required, the specification is here: https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md

Even is reading the full document would be a good idea to deeply understand the format, if you are in a hurry you should use and abuse CTRL+F to find useful information about the fields you will use.

Most of the fields of a glTF file ("buffers", "nodes", etc) are directly available in the `tinygltf::Model` instance. The only exception I found for now is `scene` which is stored in `model.defaultScene` (and I don't know why).

<span class="todo badge"></span> Take a look at the quick reference card and try to understand the sections "Concepts", "scenes, nodes", "meshes" and "buffers, bufferViews, accessors". We will start by focusing on these. 

## Buffer Objects and Vertex Array Objects

To understand the close relationship between glTF data and OpenGL, a reminder about Vertex/Index Buffer Objects (VBO, IBO) and Vertex Array Objects (VAOs) can help.

To be rendered on the GPU, geometry data is stored in buffer objects. Buffers objects correspond to buffers allocated on the GPU's memory, for quick access by the GPU.

For clarity, we call a buffer object used to store vertex data (positions, normals, texture coordinates, ...) a vertex buffer object (VBO) and a buffer object used to stored index data an index buffer object (IBO).

OpenGL can't do anything with buffer objects: they are just bytes of data for OpenGL. Like a `void*` and a byte size would just be a pointer to an area of memory with a specific size. We can't do nothing with it if we don't know the structure of the data inside.

For OpenGL to correctly interpret the data, we need to describe its structure. And for that we use **Vertex Array Objects** (VAOs). A VAO is used to describe the data of a primitive to draw (a primitive can be composed of triangles, points or lines). A primitive is described by its vertices, and each vertex is defined by vertex attributes.

So the VAO of a primitive describes:
- What vertex attributes (position, normal, ...) are enabled. Each one identified by an index (that we generally choose based on our shaders, more on that later).
- For each vertex attribute, what buffer object store its data, and how to read it (what type, how much components per attribute, where the data starts, how much bytes there is between 2 attributes).
- If the primitive has indices, what buffer object stores the indices and what is the type used (uint8, uint16 or uint32).

OK, that's a lot of concepts, read multiple time if needed.