---
id: gltf-viewer-03-camera-control
title: WIP - Camera Control
---

> Work in Progress.

Our viewer is now able to render 3d models with a basic normal shader. You can also control the camera with a "First Person Camera Controller". While this kind of controller can be good for video game, we generally don't use that for production or viewer applications.

A more common camera controller is the TrackballCamera. We will implement that in the second part of this section. We will first try to compute a better default camera when no camera is specified on the command line.

## The Camera model

A camera is generally represented by three points:
- an eye: the position of the camera
- a center: where the camera is looking at
- an up vector: which direction is up relative to the axis [eye, center]

These three points are enough to compute a view matrix with the `glm::lookAt` function.

It's important to understand that the Camera model is more powerful than the concept of matrices: you can compute a single view matrix from a camera, but for a single view matrix, there is an infinite number of camera (all cameras with the same position, orientation, but with a different center along the camera z axis).

That's why we choose to manipulate a Camera from the inputs rather than a view matrix.

In terms of code, the Camera class is defined in cameras.hpp. I've implemented a bunch of operations for reference but it's likely we won't use them. Each operation correspond to the terminology of real cameras, and you can find more information atthe following link and in the figure below.
- http://learnwebgl.brown37.net/07_cameras/camera_movement.html

![camera_movements](/openglnoel/img/gltf/camera.png)

## Implementing a better default camera

Right now our default camera is simple:

```cpp
// Somewhere in the run() function
cameraController.setCamera(
        Camera{glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)});
```

This correspond to the default view of OpenGL. It gives an identity view matrix (by default we look at the negative z axis with OpenGL).

Our goal is to implement a default camera that is better for our scene. It will not be amazing for some scenes, but most standard objects will be well presented.

The idea is to use the bounding box of the scene and compute our parameters from it. A bounding box is defined by a lower point bboxMin and higher point bboxMax. We want to have our center point at the center of the bounding box of the scene, and to look at it from the diagonal, far enough to see most of the scene.

<span class="todo badge"></span> What is the formula for the center point of the bounding box defined by bboxMin and bboxMax ?

<span class="todo badge"></span> What is the formula for the diagonal vector going from bboxMin to bboxMax ?

To compute the bounds of the scene, I've implemented a function in `utils/gltf.hpp`:

```cpp
void computeSceneBounds(
    const tinygltf::Model &model, glm::vec3 &bboxMin, glm::vec3 &bboxMax);
```

(the function is not too complicated, but it is quite similar to what we already did to build VAOs and draw the scene, you can take a look if you want or even try to reimplement it without looking).

<span class="todo badge"></span> After loading the scene, use this function to compute the bounding box of the scene.

> For the next todos you might need to reorder a bit the run() function: load the scene before computing matrices.

<span class="todo badge"></span> Replace the default camera with a camera such that center is the center of the bounding box, eye is computed as center + diagonal vector, and up is (0, 1, 0). (ideally the up vector should be specified with the file, on the command line for example, because some 3d modelers use the convention up = (0, 0, 1)).

<span class="todo badge"></span> Test your code on Sponza and DamagedHelmet.

A special case we need to handle is flat scenes (like a single triangle). In that case, we need to use align our view with a vector orthogonal to the scene. We will only handle flat scenes on the z axis.

<span class="todo badge"></span> What is a condition to test if the scene is flat on the z axis ?

<span class="todo badge"></span> It the scene is flat on the z axis, use this eye point instead: center + 2.f * glm::cross(diag, up);

To finish improving our camera, we will also improve the projection matrix and the controller speed.

<span class="todo badge"></span> Use the diagonal vector to compute a maximum distance between two points of the scene. If it is zero, set it to 100. instead.

<span class="todo badge"></span> Use near = 0.001f * maxDistance and far = 1.5f * maxDistance to compute the project matrix (the call to `glm::perspective`).

<span class="todo badge"></span> Set the camera controller speed (second argument of the controller) to a percentage of the maxDistance. Try different values and choose one that suits you.

---

Solution:
- https://github.com/Celeborn2BeAlive/gltf-viewer-tutorial/commit/8a5fb770286ae51e61471c46345c8abb8d2da2ad

## Trackball camera controller

Now our goal is to implement a new camera controller. The trackall controller should offer the following functionalities:

- Rotate around center with middle mouse pressed
- Move on plane orthogonal to the view axis with shift+middle mouse pressed
- Dolly in/out with ctrl+middle mouse pressed

It you have Blender 2.8 installed, this are the control offered by the camera.

<span class="todo badge"></span> Implement the TrackballCameraController class with an interface similar to the FirstPersonCameraController. Implement the update method to perform the required control.

Here I'm giving less information, you should take inspiration from the FirstPersonCameraController class, use the GLFW documentation to handle inputs, and implement the functionality.

<span class="todo badge"></span> In the run() method, use your new controller instead of the old one.

<span class="todo badge"></span> In the GUI, propose a radio button or a dropdown to select either the First Person Camera or the Trackball Camera