---
id: gltf-viewer-02-gltf-03-drawing
title: Drawing the scene
---

Now to we need to implement the drawing function that is called in the render loop. The render loop is already implemented with some basic code. Again we'll come back later to the code already here, for now we will focus on the drawing function.

## The drawing function

The drawing function is implemented in a lambda function in order to be used from multiple places:
- the render loop
- the render output image mode (for tests)

Similarly to VAO initialization, the drawing function will require many indirections. Our goal is to traverse the scene graph, which is composed of nodes.

The nodes can refer to a mesh or a camera. For now we are only interested in nodes that refer to meshes. We need to use the matrix of the node and its mesh to draw it on screen.

Each node can have children, that must be processed the same way. This recursive structure for the data naturally leads to a recursive function to implement. We will put it in a std::function that is already declared in the drawing function.

Right now your drawing function should look like this:

```cpp
// A lambda function (C++11)
const auto drawScene = [&]() {
  glViewport(0, 0, m_nWindowWidth, m_nWindowHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  const auto viewMatrix = camera.getViewMatrix();

  // The recursive function that should draw a node
  // We use a std::function because a simple lambda cannot be recursive
  const std::function<void(int, const glm::mat4 &)> drawNode =
    [&](int nodeIdx, const glm::mat4 &parentMatrix) {
      // TODO Implement this
    }
  };

  // Draw the scene referenced by gltf file
  if (model.defaultScene >= 0) {
    // TODO Implement this
  }
};
```

We will first implement the second `// TODO Implement this` because it is easier.

<span class="todo badge"></span> For the second `// TODO Implement this`, implement a loop over the nodes of the scene (`model.scenes[model.defaultScene].nodes`) and call drawNode on each one. The parentMatrix for them should be the identify matrix `glm::mat4(1)`.

## Matrix reloaded

Before implementing the `drawNode` function, a reminder about matrices for 3D can be useful.

Most 3D objects are created by artists in a local space: the center of the scene is at the center of the object and axis are oriented in a coherent way relative to the object.

To put an object defined locally in a global 3D scene, at an arbitrary position, orientation and scale, we must apply a matrix to the coordinates of its vertices. This matrix is called the **Model matrix** of the object (also called **localToWorld matrix**, which is a better name in my opinion).

Each object of the global scene has its own Model matrix. If M denotes a model matrix and p_l a point in the local space, then p_w = M * p_l is the position of p in world space.

What is really of interest for us, however, is the view space, the space of the camera. To convert our points from world space to view space, we use the **View matrix** (also called **worldToView matrix**), which is computed from the parameters defining the camera (often with lookAt(eye, center, up) function).

Similarly, if V denotes the view matrix and p_w a point in world space, then p_v = V * p_w is the position of p in view space.

Finally, we have the **Projection matrix** than is used to project points on the screen (a few more operations are performed by the GPU before having real pixel coordinates).

We can combine all these matrices to define new ones that are directly used by our vertex shaders:
- modelViewMatrix = view * model
- modelViewProjectionMatrix = projection * modelViewMatrix

Another matrix must be computed to transform normals. I will not go into details, but normals need a special treatment in order to maintain the orthogonality with tangent vectors. The normalMatrix is defined by:
- normalMatrix = transpose(inverse(mvMatrix));

These 3 matrices will need to be send to the GPU with `glUniformMatrix4fv` later.

## The drawNode function

Now we can attack the `drawNode` function. The first step is to get the node (as a `tinygltf::Node`) and to compute its model matrix from the parent matrix. Fortunately for you, I included a helper function `getLocalToWorldMatrix(node, parentMatrix)`. If you are interested you can take a look at the code, its mostly calls to glm function to perform the right maths.

<span class="todo badge"></span> In the drawNode function, get the node associated with the nodeIdx received as parameter. Create a `glm::mat4 modelMatrix` and initialize it with `getLocalToWorldMatrix(node, parentMatrix)`.

Then we need to ensure that the node has a mesh:

<span class="todo badge"></span> Add an `if` statement to test if the node has a mesh (`node.mesh >= 0`).

Now in the body of the `if` statement:

<span class="todo badge"></span> Compute modelViewMatrix, modelViewProjectionMatrix, normalMatrix and send all of these to the shaders with glUniformMatrix4fv.

Next step is to get the mesh of the node and iterate over its primitives to draw them. To draw a primitive we need the VAO that we filled for it.

Remember that we computed a vector `meshIndexToVaoRange` with the range of vertex array objects for each mesh (this range being an offset and an number of elements in the `vertexArrayObjects` vector). Each primitive of index `primIdx` of the mesh should has its corresponding VAO at `vertexArrayObjects[vaoRange.begin + primIdx]` if `vaoRange` is the range of the mesh. 

<span class="todo badge"></span> Get the mesh and the vertex array objects range of the current mesh.

<span class="todo badge"></span> Loop over the primitives of the mesh

Inside the loop body:

<span class="todo badge"></span> Get the VAO of the primitive (using `vertexArrayObjects`, the vaoRange and the primitive index) and bind it.

<span class="todo badge"></span> Get the current primitive.

Now we need to check if the primitive has indices by testing `if (primitive.indices >= 0)`. If its the case we should use `glDrawElements` for the drawing, if not we should use `glDrawArrays`.

<span class="todo badge"></span> Implement the first case, where the primitive has indices. You need to get the accessor of the indices (`model.accessors[primitive.indices]`) and the bufferView to compute the total byte offset to use for indices. You should then call `glDrawElements` with the mode of the primitive, the number of indices (`accessor.count`), the component type of indices (`accessor.componentType`) and the byte offset as last argument (with a cast to `const GLvoid*`).

<span class="todo badge"></span> Implement the second case, where the primitive does not have indices. For this you need the number of vertex to render. The specification of glTF tells us that we can use the accessor of an arbritrary attribute of the primitive. We can use the first accesor we have with:
```cpp
const auto accessorIdx = (*begin(primitive.attributes)).second;
const auto &accessor = model.accessors[accessorIdx];
```
Then call `glDrawArrays`, passing it the mode of the primitive, 0 as second argument, and `accessor.count` as last argument.

We then have one last thing to implement, after the `if (node.mesh >= 0)` body: we need to draw children recursively.

<span class="todo badge"></span> After the `if` body, add a loop over `node.children` and call `drawNode` on each children. The matrix passed as second argument should be the modelMatrix that has been computed earlier in the function.

## Conclusion

Now, if everything is OK in your code you should be able to see something by running your program on a scene. My advice is to use the Sponza scene (stored in https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/Sponza). If you don't have enough disk space to download the whole repository, you can pick it here: [Sponza](/openglnoel/files/Sponza.zip).

To run the program in viewer mode, you need to run in your terminal:
```bash
cd PATH/TO/PROJECT_ROOT/build-gltf-viewer-tutorial # We have to be in the build folder
# Adapt the path to the file to your case:
./bin/gltf-viewer viewer ../glTF-Sample-Models/2.0/Sponza/glTF/Sponza.gltf --lookat "-5.26056,6.59932,0.85661,-4.40144,6.23486,0.497347,0.342113,0.931131,-0.126476"
```

It should run the program and set a camera such that you see:
