# Checkpoint 3: OpenGL renderer

## Current OpenGL implementation

The live program now creates an SDL2 OpenGL context and renders filled terrain
and OBJ tree faces with a depth buffer and a directional sun light. The OBJ
loader reads vertices, vertex normals, and polygon faces, including Blender's
`vertex//normal` format; polygons are triangulated for drawing. The renderer
uses the project `Mat4` transforms and keeps the SDL software wireframe path
available with `./forest --wireframe`. The canopy animation remains a temporary
scale pulse until the season state is connected.

## GitHub link and what remains

Branch: https://github.com/AsiyaKondusa/seasonal-forest/tree/gl-renderer

The OpenGL code still needs a runtime build and visual check in the project's
WSL/Linux environment with SDL2 and OpenGL development packages installed. The
remaining integration is to use Person B's final models, Person C's seasonal
colors and canopy scale, and Person D's particle drawing. The current program
uses fixed tree colors and a temporary canopy pulse.
