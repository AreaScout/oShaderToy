oShaderToy
===========

Simple console tool to test OpenGL ES 2.0 fragment shaders on Ordoid boards.

Changed to run on odroid boards by Daniel Mehrwald, inital work by dff180 for Raspberry Pi

Fragment shader and optional texture are specified on command-line.

Sample video: https://www.youtube.com/watch?v=F0mfntGDmxg

Credits
-------

* EGL class code by [Jon Macey](http://jonmacey.blogspot.de/2012/06/opengl-es-on-raspberry-pi-pt-3-creating.html)

* Sample shaders from [ShaderToy](https://www.shadertoy.com) by Inigo 'iq' Quilez. Explore the ShaderToy website for additional shader and texture usage examples.

* Sample textures from http://elout.home.xs4all.nl/texl/tex/index.html

Installation
------------
Note: The following instructions are tested on the new armsoc driver from dsd

1. Get sources with `git clone https://github.com/AreaScout/oShaderToy.git`
2. Install dependencies with `sudo apt-get install libsoil-dev`
3. `cd oShaderToy`
4. `make`

### Examples

* Using default fragment shader

    `./oShaderToy`
    
* Using fragment shader which doesn't need texture

    `./oShaderToy shaders/flower.f.glsl`
    
* Using fragment shader with texture

    `./oShaderToy shaders/relieftunnel.f.glsl textures/texl0.jpg`
    
