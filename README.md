# Ocean Surface

Authors: Jiashuo Li, Xie He, Ting Gong

A real-time ocean surface simulated using Tessendorf's algorithm, rendered with **OpenGL**. This is the team project for USC CSCI-580 Computer Graphics in Fall 2015. 

This project is only designed for **Windows** and should be compiled with **Visual Studio 2015**.

## Libraries Used
* `FFTW` for Fast Fourier Transform.
* `GLFW` for window management.
* `GLEW` for GLSL.
* `SOIL` for screen capturing.
* `FFmpeg` for converting screenshots to video.

## Framework
The framework is based on the code from [Learn OpenGL](http://www.learnopengl.com/).

## Algorithm
The algorithm is based on Tessendorf's paper [*Simulating Ocean Water*](http://graphics.ucsd.edu/courses/rendering/2005/jdewall/tessendorf.pdf). 

## Our Paper
You can also find our paper *Simulating Ocean Surface* under `release`. It explains the implementation in detail.

## Result

We made two videos for [a wavy surface](https://youtu.be/U2fkrXxvPRY) and [a calm surface](https://youtu.be/ewAPFoVxfFE) with N = M = 1024.

Here are some screenshots.

![](/screenshots/screenshot1.jpg)

![](/screenshots/screenshot2.jpg)

![](/screenshots/screenshot3.jpg)


