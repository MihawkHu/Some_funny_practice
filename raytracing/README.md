## Ray Tracing 

### Introduction
Using Monte Carlo ray tracing algorithm to render virtual 3D scenes. The algorithm recursively trace light rays from the light source in the scene. Use scene file in xml format and OBJ file in obj format

### Features
* Soft shadow
* Reflection and refraction
* Color bleeding
* Using octree data structure
* Using openMP to parallellize
* Can handle texture

### File structure  
* src: source code folder  
    * lib: some external dependency library  
    * \*.h: header file  
    * \*.c: implementation code  
* data: input and output file  
    * cube.obj: OBJ file  
    * scene.xml: scene file in xml format  
    * result.ppm/png: render result file  
    * CMakeLists.txt: cmake file  
    * raytrace: executable file in linux  
    * run_linux.sh: test script in linux, compile and execute  
    * README.md: this file  

### How to use  
Using cmake to make this project.   
    
    $ cmake ./ 
    $ make  
Then you will get the executable file raytrace.
Using 

    $ ./raytrace <Scene file> <Output name>
to run the ray tracing.

For example, 

    $ ./raytrace ./data/scene.xml result

Then you will get the render result file.

Also, you can use the test script to directly compile and execute this project,

    $ ./run_linux.sh  

