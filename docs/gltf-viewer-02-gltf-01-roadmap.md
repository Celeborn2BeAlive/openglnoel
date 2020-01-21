---
id: gltf-viewer-02-gltf-01-roadmap
title: Introduction and roadmap
---

## Roadmap

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

## Running your program

If everything is OK during the tutorial, you should always be able to build and run your application on a glTF file (event if nothing is displayed, only black).

My advice is to use the Sponza scene for testing (stored in https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/Sponza). If you don't have enough disk space to download the whole repository, you can pick it here: [Sponza](/openglnoel/files/Sponza.zip).

To run the program in viewer mode, you need to run this your terminal:
```bash
cd PATH/TO/PROJECT_ROOT/build-gltf-viewer-tutorial # We have to be in the build folder
# Change the path to the file if required.
./bin/gltf-viewer viewer ../glTF-Sample-Models/2.0/Sponza/glTF/Sponza.gltf --lookat "-5.26056,6.59932,0.85661,-4.40144,6.23486,0.497347,0.342113,0.931131,-0.126476"
```

Don't expect to see anything but black before the end of the part "Loading and drawing".