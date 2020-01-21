---
id: gltf-viewer-01-intro-03-devenv
title: Development Environment
---

This part is optional and only gives some advice for your development environment, especially if you want to be multi-platform. I present here my own development environment.

## What OS I use

I almost only develop on Windows. I had to switch from Linux to Windows in 2015 after my PhD when I entered at Ubisoft. If people tells you that programming on Windows is a pain, well my opinion is that they don't really know. In 2020, with all the tools that exist now, you can build a complete development environment on Windows that will behave almost like one on Linux. The only thing missing would be, I guess, third party development libraries because there is no package manager. But I've known the hell of having a Linux system with old gcc, on which you have to recompile everything (gcc included), and trust me it is the same shit as Windows in that case. So Linux might be better for learning how to code, but in the real world when you have to distribute code, or do things out of the confort zone, both Linux and Windows are hard to master.

Now I'm not defending Windows over Linux (first because Windows is not Open Source, and Windows in general might be shit). I'm just talking about how hard it is to develop on both systems.

Also I don't know anything about MacOS dev, so don't ask me.

## Compiler

On Linux I use gcc, the most recent version if possible.

On Windows I use Visual Studio Community, also the last version, which can be downloaded from the microsoft website. However I don't use the IDE, only the compiler, and with `cmake --build`.

Another good choice can be clang I guess, which is multi-platform, but I never tried it.

## Editor

I stopped using the IDE of Visual Studio quite recently, when I fully switched to Visual Studio Code.

VS Code is kind of amazing actually:
- Open source, but maintained by a very active team from Microsoft
- Multiplatform
- Built with web technologies, but quite fast
- Extensibility with extensions
- A lot of extensions already available
- Integration with debuggers
- Multi languages
- Native markdown support

The C/C++ extension of VS Code is now mature enough, in my opinion, to stop using heavy IDEs like Visual Studio. Don't get me wrong: I like coding in Visual Studio, but it is really slow and you basically need some paid extensions (Visual Assist or Resharper) to really have a complete and powerful IDE. Also it is a pain to configure.

One last thing I still miss from Visual Studio is the view of parallel thread stacks, I hope one extension would bring it to VS Code some day.

The truth is, I never used VS Code on Linux (I'm now almost fully programming on Windows, I only build my code on Linux and for that I don't need an editor). One day I may reinstall an Arch Linux on one of my computer to test it, but not right now !

## VS Code Extensions

Here are the extensions I use for C++ development and other general tooling:
- Settings Sync: the first one I always install. It sync your settings and extensions on a gist on Github, so you don't have to redo everything when you work from different computer. You have to configure it to auto download and auto upload.
- C/C++
- Clang-Format: Auto format code on save according to some rules. That way, not more debate about code conventions, the .clang-format file decide.
- CMake Tools: Not really required, it is also a bit strange and need some changes I think. But it allows to easily change build config or target to debug.
- CMake: Coloring and autocompletion
- Diff: To compare files
- Git Graph
- Git Lens
- Git Automator (quick commit and push)
- glTF Tools (view glTF models in VS Code haha)
- Json Editor (because we always need to edit some json files)
- Local History: maintain a local history of all files when you save. If you use it, think of adding a global gitignore file to you system and add .history to it.

That is all for C++ Dev, I also have many other extensions for other languages.

## Windows Dev Env, what other tools ?

- MSYS2 is my terminal
- Cmder is my terminal launcher (quake style dropdown ftw)
- Scoop is my installation manager (if a software is available in Scoop, I install with Scoop)
- Everything is my file searcher
  
I think thats all.