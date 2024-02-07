# OpenGLCourseApp

What is a rendering pipeline?
1. Rendering pipeline is series of of stages that take place in order to render an image to the screen, it's all part of the GPU
2. Four of these stages are programable via shaders. The four are vertex shader, fragment shader these are the two most important ones, the ones we will be using the most, rest is geometry shader and tessellation shader (this is fairly new), there is also compute shader but that is extremely new
3. Shaders are pieces of code written in GLSL(OpenGL Shading Language) or HLSL(High Level Shading Language) for directX
4. GLSL is based on C

The rendering pipeline stages:
1. Vertex specification
2. Vertex shader (programmable)
3. Tessellation shader (programmable)
4. Geometry shader (programmable)
5. Vertex post-processing
6. Primitive assembly
7. Rasterization
8. Fragment shader (programmable)
9. Per sample operations
Can be more or less depending on the person

Vertex specification:
Vertex is a point in space defined by xyz coordinates
A primitive is a simple shape defined using one or more vertices
We generakl