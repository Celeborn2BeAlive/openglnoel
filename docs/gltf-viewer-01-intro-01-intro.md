---
id: gltf-viewer-01-intro-01-intro
title: Introduction
---

Welcome to this tutorial ! Our goal is to learn OpenGL by building a complete project: a glTF Viewer.

## Why a viewer ?

Coding a 3D viewer to learn OpenGL has several advantages:

- Not too hard, not too easy
- Focus on 3D rendering
- The knowledge acquired is likely to be useful in the future if you work in 3D programing
- Easy to extend with new rendering techniques, lot of customization is possible
- Can become your 3D hub to try new things, build prototypes, etc.
- More fun than drawing cubes and spheres, right ?

## What is glTF ?

glTF is a file format specified by the Khronos Group, which is also responsible for the specification of OpenGL.

Here is a direct citation from the repository https://github.com/KhronosGroup/glTF where the specification is hosted:

> glTF (GL Transmission Format) is a royalty-free specification for the efficient transmission and loading of 3D scenes and models by applications. glTF minimizes both the size of 3D assets, and the runtime processing needed to unpack and use those assets. glTF defines an extensible, common publishing format for 3D content tools and services that streamlines authoring workflows and enables interoperable use of content across the industry.

Our goal is to code a viewer for these type of files, using OpenGL and C++.

## Why glTF ?

There is many 3D file format out there and most often people starts with the OBJ file format. But nowadays I think glTF is a better starting point for loading 3D model. Indeed:

- It has a good specification
- It can be extended in a way that is formalized by the specification
- It has by default a modern material model that is often used in real-time rendering softwares (games or applications)
- It has been built with OpenGL in mind: the data layout closely follows how the data is stored with OpenGL and how rendering is done
- Khronos provides a repository with many sample models to test our application: https://github.com/KhronosGroup/glTF-Sample-Models
- There is also a web sample viewer to view in 3D the sample models: https://github.khronos.org/glTF-Sample-Viewer/. Its source code is available so we can compare our code to its if we don't have expected results.
- Sketchfab (https://sketchfab.com/), a platform to share 3D models, propose auto-conversion to glTF for all its models. Many models are free so it gives us a large library of 3D models to play with our viewer.
- It has an official Blender importer and exporter https://github.com/KhronosGroup/glTF-Blender-IO which can be great to create your own assets for free.

## Prerequistes

For now this tutorial assume you already have the following basic knowledge:

- **OpenGL 3+** You should know what are buffer objects, vertex array objects, textures, shaders. The tutorial will explain how to create and manipulate this objects but will not dive into details about them. However I'll link wiki or documentation pages related to these concepts.
- **C++** You should be familiar with basic C++ syntax and semantics, know how to write a function or a class, etc. I'll introduce more advanced concepts as needed. If you think something is not clear don't hesite to contact me and I'll update the tutorial to provide more details.

## What are we going to learn ?

This tutorial covers many aspects of OpenGL and C++ development, but also code versionning and collaboration with Git, CMake and test driven development. Here is a coarse list of concepts we will cover:

- Forking, cloning and collaborating on git repositories
- Compiling a C++ project using CMake
- Create a graphical application
- Understand and manipulate glTF format
- Use OpenGL to render a 3D scene described by a glTF file
- Advanced camera control
- Advanced material representation
- Shadow Mapping algorithm
- Deferred Rendering
- Post Processing with OpenGL
- Use a test driven development architecture to test our project at each iteration

## Test driven development (TDD)

One special feature I wanted for this tutorial is test driven development. More specifically, we will use **Non Regression Unit tests** (NRU tests) to ensure our application compute correct images at each step of the development process.

For that we will use the amazing repository containing glTF samples: https://github.com/KhronosGroup/glTF-Sample-Models. Some of these samples are meant to test specific features of the format.

I will provide you reference images for each sample at each step of the tutorial, and you will be able to use an automatic procedure provided with the SDK to test your application on each glTF sample. We will code the application such that it can output the rendered image in a file, which will be compared to the reference image by the test procedure. With that you will be able to know quickly if your application does the correct thing.

We will also output logs to text files and compare them to reference logs. Using this will permit us to follow what the program is doing and where it does not do what it should do.

In our case I provide reference images, but in reality the set of reference images is built iteratively as the software is developped. Once a feature is considered to be stable and well tested, several reference images showing that feature are generated on tests scenes. If one day the code is modified and the feature is broken, developpers are notified by the automatic test procedure and they can debug it.

TDD is generally considered a good practice. It can be boring and hard to setup, but once its done it provides a good security level and offers a good feeling of safety when we change our code.

## Operating System

This tutorial has been written for Linux and Windows. I mainly code on Windows, so there may be mistakes for Linux. If you spot one, contact me.

On Linux I use gcc, and on Windows I use Visual Studio Community 2019 (more specifically, I use Visual Studio Code for code edition and only the compiler of Visual Studio Community 2019).