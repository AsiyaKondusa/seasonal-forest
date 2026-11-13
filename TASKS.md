# Remaining work (4 people)

Checkpoint 3 is the week of 16th Nov and Checkpoint 4 the week of 23rd Nov. Target: Person A, Person B, and Person C merged into `main` by Wed 18 Nov, so we can record the Checkpoint 3 GIF.

Read `README.md` first: it has setup and the full git workflow (branches, pull requests, conflicts). Every task below is done on its own branch.

## Tools
We use both OpenGL and Blender, to learn both.

| Tool | Used for |
|---|---|
| C++ with OpenGL (window from SDL2) | The live program: filled faces, depth test, lighting, seasons, particles. This is the main deliverable. |
| Blender | Tree, leaf and snowflake models exported as .obj, and a rendered four-season timelapse video. |

Our own matrices in `mat4.h` stay in use. The Checkpoint 3 GIF comes from the live OpenGL program. The Blender render is an additional demo. If we want it to count as the evidence instead, ask the lecturer first.

## What each person provides to the others
Agree on these names early and stick to them. Details are for each owner to decide, but post the final names in the group chat.

| Who provides | What | Who uses it |
|---|---|---|
| Person A, in `gl_renderer.h` | A way to draw a mesh with a model matrix and an RGB color, and a way to draw a small point or square at a position with a color | Persons B, C, and D |
| Person B, in `models/` | Low-poly `.obj` files (`trunk.obj`, `canopy.obj`, `leaf.obj`, `snowflake.obj`) with proper normals and pivots | Person A (rendering), Person C (timelapse), Person D (particles) |
| Person C, in `season.h` | A function `getSeasonState` that takes elapsed seconds and returns the current season, progress through it (0 to 1), canopy scale, canopy color, ground color and sun direction | Person A (colors and light), Person D (season), main loop (canopy scale) |
| Person D, in `particles.h` | `updateParticles` (takes time step and season) and `drawParticles` | Main loop |

## Blender export rules (everyone who exports)
- Keep each model low-poly: trunk under about 50 faces, canopy under about 200, leaf and snowflake 1 or 2 faces.
- Export trunk and canopy as **separate** files named `trunk.obj` and `canopy.obj`.
- Put the model origin at the bottom of the trunk. The canopy must start where the trunk ends. Our `canopyMatrix` scales the canopy around y = 1, so either keep the canopy base at y = 1 or change that pivot in `main.cpp` and tell everyone.
- File > Export > Wavefront (.obj). Tick **Selection Only** and export each object separately. Tick **Triangulated Mesh** and **Normals**. Untick materials and UVs. Forward axis **-Z**, up axis **Y**. (Option names vary slightly by Blender version.)
- Test the file in our wireframe build before committing it. If the tree is sideways or huge, fix scale or axis in Blender.
- Keep `.blend` source files in `models/` (or `blender/` for the timelapse).

## Person A (name: ______): OpenGL renderer
Branch: `gl-renderer`. Files: `gl_renderer.h`, `gl_renderer.cpp`.

1. Install `libgl1-mesa-dev` and check OpenGL works in your setup (for example with `mesa-utils` and `glxinfo -B`).
2. Create the window with an OpenGL context, link OpenGL in the Makefile.
3. Extend the OBJ loader and `Mesh` to keep faces and normals. Blender writes faces as `vertex//normal` pairs and normals on `vn` lines.
4. Draw terrain and trees as filled polygons (`GL_POLYGON` or `GL_TRIANGLES`).
5. Enable `GL_DEPTH_TEST` so overlapping surfaces are handled automatically.
6. Add a directional light for the sun using the normals.
7. Reuse our matrices (remember the transpose, see README section 9).
8. Keep the wireframe path working until the OpenGL version is stable.
9. Checkpoint 3: write "Current OpenGL implementation" and "GitHub link and what remains".

## Person B (name: ______): 3D Models & Scene Assets
Branch: `models`. Files: `models/`.

1. In Blender, make the tree models (`trunk.obj` and `canopy.obj`) following the export rules above, and push them by Mon 16 Nov because Person A and Person C need them.
2. Make a leaf (`leaf.obj`, 1 or 2 faces) and a snowflake (`snowflake.obj`, 1 face) for Person D and Person C.
3. Test all exported `.obj` files in the existing wireframe build to ensure scale, orientation, and pivots are completely accurate before opening a pull request.
4. Help Person A test mesh loading once the OpenGL renderer is ready, and tune model proportions/terrain layout if needed.
5. Checkpoint 3: help Person A and C capture the animation GIF and verify model normals/shading.

## Person C (name: ______): Seasons, Colors & Blender Timelapse
Branch: `seasons`. Files: `season.h`, `blender/`.

1. Build the season state machine that loops forever, each season lasting a few seconds.
2. Implement linear interpolation (lerp) between colors: canopy green to orange to brown in Autumn, white ground in Winter.
3. Handle seasonal transforms and lighting: canopies grow from small to full in Spring, and low sun angle in Winter.
4. Replace the sine-wave pulse in `main()` with the season's canopy scale. You can prototype now in the wireframe build by changing the SDL line color per season, without waiting for OpenGL.
5. Blender timelapse: once Person B pushes the models, build the forest in Blender, animate the four seasons (leaf color, canopy size, ground color), add falling leaves and snow with Blender's particle system, orbit the camera, and render a 20 to 30 second video. (Optionally automate with a `bpy` Python script saved in `blender/`.) Share the video by Drive link, not in git.
6. Checkpoint 3: write color interpolation text and capture the animation GIF with Person A. Checkpoint 4: show that the season loop runs continuously.

## Person D (name: ______): Particles & Final Integration
Branch: `particles`. Files: `particles.h`, `particles.cpp`.

1. Create a particle pool of fixed size (for example 500 particles), each with position, velocity, and an active flag.
2. Set seasonal behavior: falling leaves in Autumn, falling snow in Winter, and none in Spring and Summer.
3. Update physics: each frame lower each particle a little, with a small sideways sway.
4. Implement memory-reuse optimization: when a particle reaches the ground, reuse it at the top instead of making a new one.
5. Draw particles as points or tiny squares (or using Person B's leaf/snowflake meshes) through `drawEdge3D` first, and switch to OpenGL once Person A's renderer is merged.
6. Final integration with everyone: lead the final merge of all branches, fix cross-branch bugs, tune winter lighting, and record the final demo.
7. Checkpoint 4: write the final demonstration, what works, what does not, and final changes (with everyone reviewing).

## Checkpoint text, who writes what

| Checkpoint item | Writer |
|---|---|
| Checkpoint 3: current OpenGL implementation | Person A |
| Checkpoint 3: animation evidence (GIF) | Person C with Person A & B |
| Checkpoint 3: GitHub link and what remains | Person A |
| Checkpoint 4: final demo, what works, what does not, final changes | Person D, everyone reviews |

## Timeline
- **Sat 14 to Sun 15 Nov:** Everyone does setup from the README and picks a role. Person B starts the Blender models.
- **Mon 16 to Tue 17:** B pushes all `.obj` models early; A builds the OpenGL renderer; C builds the season logic; D builds the particle pool.
- **Wed 18:** Merge A, B, and C into `main`, record the Checkpoint 3 GIF.
- **Thu 19 to Fri 20:** Write Checkpoint 3 text; C starts the Blender timelapse render.
- **Week of 23rd:** Merge D (particles), finish Blender timelapse, fix bugs, optimize, record the final demo.

## Definition of done (for each person)
- Your branch builds with `make` and runs without crashing.
- It is merged into `main` through a reviewed pull request.
- Your checkpoint text is written.
- You posted in the group chat what you changed and any names others need.

## Shared rules
Follow the git workflow in `README.md` sections 4 to 6. Never push to `main` directly, and each person works only on their own branch and files.
