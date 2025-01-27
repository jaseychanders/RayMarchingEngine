### Final Project
## Jasey Chanders

## CSCI4229/5229 Fall 2024

This project is a demonstration of using a frag shader to draw objects and shows off some of the cool ways you can combine them that are simple
in ray marching but would be incredibly difficult in standard basic object based graphics. This code is essentially a basic rendering engine that
allows the user to play with 5 different 3d objects (sphere, cube, approximation of Sierpinski's triangle, pyramid and prism). For each object you are able
to change its properties such as it's visibility, location and size as well as some specific properties for certain shapes. The Cube allows for a smoothness factor
as well as destructive interference with the sphere. The Sierpinski's triangle allows for rendering at 4 different fractal levels.  Beyond basic properties the overall method for combining objects
can be changed from a simple combination to a smoothing effect called a smooth minimum.

## Hot keys were provided to show the most interesting effects.
0: Reset Camera
1: Object Addition
2: Smooth Addition of objects
3: Smoothness of a single object using a different method of smoothing
4: Destructive Addition of a cube and sphere
5: Smooth Addition of the fractal and a sphere
6: My best approximation of a Christmas tree with all the shapes (Tis the season :)

By far the most complicated object I rendered is the Sierpinski's triangle. It is built by combining the sdf of a pyramid,
a very thin rectangle and destructive addition of two upside down prisms. This building block is then scaled down and used
to build the bigger fractal. This does not have arbitrary depth and the correct way to render this would be to come up with
a 3D Sierpinski's SDF which I'm sure someone has but in the interest of my own learning and my lack of incredible understanding
of building complex SDFs I decided to build it myself in a way I could get my head around.


## Key bindings (All shown on screen for ease of use as their are a lot)
  arrows       Controls view of scene
  n            Flips though possible shapes to edit
  0            Reset camera angle
  1-6          Hot keys to view interesting scenes
  v            Toggle visibility of object
  w/a/s/d/z/e  Move object in 3D space
  +/-          Change size of object
  x            Toggle render method (simple addition or smooth addition)
  g/t          Adjust smoothness of shape
  r            Set cube to destructive interference mode  (Only when combined with Sphere)
  m            Changes fractal level of the Sierpinski's Triangle
  ESC        Exit


  Deviations from plans:
  For the Fractal I was not able to render it at more than 4 fractal levels because my method of building it slowed down the shader too much at higher levels.
  I strayed pretty far from my original idea. Wanting to render fractals led me to shaders. Wanting to do 3D fractals led me to ray marching. Then I got super fascinated with
  ray marching and ended up spending most of my time building a ray marching playground with all the different effects you
  can build with it. So ultimately the project ended up being much more about ray marching than fractals but I learned a massive
  amount as this is my first time doing anything graphics and I'd never heard of a shader before this class. I thoroughly enjoyed
  the project and I have infinitely more ideas of ways I would add to it and improve it if I had more time but sadly the 40-50 hrs I I put into
  this were all I had (and a bit more). Thanks for the freedom to design our projects!


-----------
## Sources

What I used from sources:
Basic ray tracing algorithm
Basic SDFs
smoothMin and smoothMax functions
Lots of ideas on how to do ray marching

What I did
Modify ray tracing algorithm to fit my needs including modifications for screen size and xyz bounds as well as removing
the default background color
Added Camera control to allow scene to be viewed from different locations
Modify SDFs to allow for xyz offsets and mirroring/rotation
Create combinations of sdfs to build fractal at different fractal depths
Create user controls and UI
Merge smoothMin and smoothMax functions and create a color mixing algorithm
Add color handling of objects (by default all objects in the scene are compressed into just a distance so separate coloring
    is non trivial)
A lot of research on how ray marching and sdfs work
A lot of trying to derive my own sdfs. It turns out anything involving
triangles is super complicated so I decided it wasn't the best use of my time
Merging scheme for optionally rendering objects with user set properties
Creating hot keys to see interesting scenes
Destructive Addition of Cube

Known Bugs:
Not all effects work on all objects so I had to take away the options for those effects on certain objects
The color mixing for smooth min over saturates
There is something funny with the normals on the edges of the fractal. The normals are based on online sources and
should work correctly (they are logical to me) but I'm unsure were the few black pixels are coming from.


General References:
Examples from class, previous assignments for this class, occasional questions to chatGPT for glsl syntax but not for
blocks of code

Idea references:
https://en.wikipedia.org/wiki/Fractal-generating_software
https://www.youtube.com/watch?v=f4s1h2YETNY
https://www.youtube.com/watch?v=dzymDsEUjKA
https://www.youtube.com/watch?v=Cp5WWtMoeKg
https://www.youtube.com/watch?v=LyQWZRfWotQ
https://numfactory.upc.edu/web/Geometria/signedDistances.html
https://www.youtube.com/watch?app=desktop&v=BNZtUB7yhX4
https://timcoster.com/2020/03/05/raymarching-shader-pt5-colors/

Code references:
https://www.youtube.com/watch?app=desktop&v=BNZtUB7yhX4
https://timcoster.com/2020/02/11/raymarching-shader-pt1-glsl/ - ray marching
https://iquilezles.org/articles/distfunctions/ - sdfs
https://www.shadertoy.com/view/Nld3DB - sdfs
Occasional use of ChatGPT for c and glsl syntax
