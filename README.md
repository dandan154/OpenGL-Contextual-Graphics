# OpenGL-Contextual-Graphics
The C++ &amp; GLSL code from a graphics program designed to display a house with a skybox, lit using a variety of lighting techniques from a dynamic light source. 

Textures and objects have been excluded due to licensing. 

## Functionality
The goal of this initial program was to load and display a .obj file of a house which was then placed within the context of a landscape and a skybox (which were also loaded .obj files). These loaded objects were then lit using a variety of dynamic lighting techniques which could be examined through an interactive display window, where the user could move about the scene created within the program. 

## Techniques
Below is a sampling of the techiques employed throughout development: 

* Normal Mapping - Used for the house; provided illusion of depth, without increased object complexity. 

* Uniform Buffers - Groups shader variables together to ensure more efficient data transfer between main program and shaders. 

* Per-Fragment Shading - This was used in place of Gourand shading, this means that the colour of individual pixels was calculated instead of individual polygon vertices (i.e. geometry that might cover more that one pixel).    



## Libraries Used

* [SOIL2](https://bitbucket.org/SpartanJ/soil2/src/default/) - Texture Loader, allowed .PNGs to be applied to objects.

* [GLEW](http://glew.sourceforge.net/install.html) - OpenGL Extension Wrangler Library.

* [GLFW](https://www.glfw.org/) - OpenGL Implementation Library.
