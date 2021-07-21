# COMP 371 Assignments

## Build Instructions

### Command Line

1. Download and Install CMake
2. Open a terminal
3. Run cmake:

```
cd <source_folder>
cmake -S . -B <build_folder>
cmake --build <build_folder> --target install
```

**NOTE:** In the case of cmake the `<source_folder>` is the topmost folder with
a CMakeLists.txt file.

**NOTE:** The `<build_folder>` can be anything but is usually just named `build`.


### CMake GUI

You can use the CMake GUI to configure and generate the Visual Studio Project.

1. Download and Install CMake
2. Open the CMake GUI
3. Set the `Where is the source code:` field to the source folder
4. Set the `Where to build the binaries:` field to different location (e.g.: source_folder/build)
5. Click Configure
6. Click Generate

#### Windows

Once that is created, you can build the **ALL_BUILD** project followed by building
the **INSTALL** project.

#### Linux

In the terminal:

```
cd <build_folder>
make install
```


## Running

Once built and installed, all the relevant files will be in the **dist** folder
at the top level of the source folder.

### Linux/macOS

In the terminal type:

```
./lab02
```

### Windows

Click on the **lab02.exe** from the File Explorer.

### Operation Manual
```
COMP371 Assignment1

This is a program displaying 3D object comprising of multiple cubes and a wall with matching hole.

User input interactions as below:
    for keyboard:
        press "U/J": object model scale up/down
        press "+/-": object model and corresponding wall scale up/down
        press "A": move left
        press "D": move right
        press "W": move up
        press "S": move down
        press "a": rotate left 5 degrees about Y axis
        press "d": rotate right 5 degrees about Y axis
        press lefe/right/up/down arrow: change world orientation
        press "Home": reset world to initial position and orientation
        press "P": change rendering mode to points
        press "L": change rendering mode to lines
        press "T": change rendering mode to triangles
        press "Space": reset model to initial position and orientation
    for mice:
        mouse moving up and down in y direction: camera tilt
        mouse moving left and right in x direction: camera pan
```

## References

Good place to learn OpenGL
https://www.learnopengl.com

Official OpenGL 4 Reference Documentation
https://www.khronos.org/registry/OpenGL-Refpages/gl4/

Official OpenGL Wiki
https://www.khronos.org/opengl/wiki/

Easy to Navigate OpenGL Function Documentation
http://docs.gl/

GLM Documentation
https://glm.g-truc.net/0.9.9/index.html

GLFW Documentation
https://www.glfw.org/

Good place for advanced graphics theory
https://www.scratchapixel.com/

