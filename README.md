# Xlib + Vulkan 1.0

Build shaders:
```sh
glslangValidator -V triangle.vert -o triangle.vert.spv

glslangValidator -V triangle.frag -o triangle.frag.spv
```

Build:
```sh
gcc -O3 -o triangle main.c engine.c -lX11 -lvulkan -lm

gcc -g3 -Wall -Wextra -Wdouble-promotion -fsanitize=address,undefined -o triangle main.c engine.c -lX11 -lvulkan -lm
```

Run:
```sh
./triangle
```

Result (note high FPS rates come from new vacant images present for vsync triple buffering):

![Triangle rotation GIF](triangle.gif)
