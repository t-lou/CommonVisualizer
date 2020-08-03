# CommonVisualizer

Sometimes when we want to do simulation or computation in C++, it is not easy to visualize the data, especially transforms.

This project aims at providing a header-only library for data visualization with OpenGL, such as mesh, point cloud and simple shapes. The hpp header can also be easily built as library (examples/test_sep_*).

# Dependencies

- freeglut
- glm
- glew
- glfw3
- libpng++
- eigen3 (for one example code only)

# Steps

- download
- include CommonVisualizer.hpp
- build with gl-libs

# Interactions

- **w-s-a-d** rotate
- **n-f** zoom
- **up-down-left-right** translate
- **left-mouse-button** click-drag-release to rotate
- **middle-mouse-button** click-drag-release to zoom (only up and down)
- **right-mouse-button** click-drag-release to translate

# Screenshots

- supported primitives
![primitives](https://github.com/t-lou/CommonVisualizer/blob/master/examples/screenshots/test_all.png)
- visualization of boxes fall into crate
![primitives](https://github.com/t-lou/CommonVisualizer/blob/master/examples/screenshots/test_bullet_crate.png)
- visualization of spheres
![primitives](https://github.com/t-lou/CommonVisualizer/blob/master/examples/screenshots/test_spheres.png)
- screenshot generation
![primitives](https://github.com/t-lou/CommonVisualizer/blob/master/examples/screenshots/test_screensaver.png)
