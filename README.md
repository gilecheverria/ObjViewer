# ObjViewer
Program to view OBJ models in 3D, using OpenGL

## Compiling

Use the provided makefile:

```bash
make
```

## Running

Use the executable named `objReader`, followed by the path to an OBJ file.

```bash
./objReader <path_to_obj>
```

Once inside the program, these are the available controls:
- Left Mouse Button and drag: rotate the view
- Middle Mouse Button and drag: pan the view
- Right Mouse Button and drag: zoom in/out
- Z key: Reset rotation around X axis
- X key: Reset rotation around Y axis
- C key: Reset zoom
- V key: Reset pan

- 1 key: toggle FPS display
- 2 key: toggle backface culling
- 3 key: toggle wireframe overlay
- 4 key: toggle axis display

- L key: toggle lighting
- K key: toggle flat or smooth shading
- M key: toggle wireframe display
- N key: toggle display of vertex normals

- ESC key: exit the program

## TODO
- [ ] Fix keyboard inputs not working
- [ ] Switch to GLFW https://www.glfw.org/
