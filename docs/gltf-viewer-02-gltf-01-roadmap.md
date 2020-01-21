---
id: gltf-viewer-02-gltf-01-roadmap
title: Introduction and roadmap
---

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