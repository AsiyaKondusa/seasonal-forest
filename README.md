# Seasonal Forest

A 3D seasonal forest timelapse for our computer graphics unit: Spring, Summer,
Autumn and Winter across multiple trees and a terrain grid. Phases 1 and 2 are
built in C++ with SDL2 using our own math (no OpenGL). Phase 3 onward uses
OpenGL for the live program and Blender for models and a rendered timelapse.

Repository: https://github.com/AsiyaKondusa/seasonal-forest

## Current status

Done: Vector3 and Mat4 (`mat4.h`), perspective projection, Bresenham lines,
terrain grid, OBJ loader, 7 trees each with its own model matrix, orbiting
camera, canopy scaling around its base.

Remaining work and who does what: see `TASKS.md`.

## 1. Install what you need

### Windows (use WSL)

1. Open PowerShell as administrator and run `wsl --install`. Restart the
   computer when asked, then open "Ubuntu" from the Start menu and create a
   Linux username and password.
2. In Ubuntu, check that Linux windows can open on Windows:
   `echo $DISPLAY`. It should print something like `:0`. If it prints nothing,
   run `wsl --update` in PowerShell, restart, and try again. You need
   Windows 11 or a recent Windows 10.
3. Install the tools (see "Ubuntu / Linux" below).

### Ubuntu / Linux (also inside WSL)

```
sudo apt update
sudo apt install build-essential libsdl2-dev git
```

Check it worked: `g++ --version` and `git --version` both print a version.

### macOS (untested)

```
brew install sdl2
```

### Blender (everyone who makes models or the Blender render)

Download the current stable version from https://www.blender.org and install it
on Windows (it does not need to be inside WSL). To open files that live inside
WSL, use this folder path in Blender's file dialogs:
`\\wsl.localhost\Ubuntu\home\YOUR_LINUX_USERNAME\seasonal-forest`
(if that fails, try `\\wsl$\Ubuntu\home\YOUR_LINUX_USERNAME\seasonal-forest`).
From Ubuntu, `explorer.exe .` opens the current folder in Windows Explorer.

### VS Code (optional, editor)

Install VS Code on Windows, then install its "WSL" extension. In Ubuntu run
`cd ~/seasonal-forest` then `code .`. The first run downloads a helper and takes
a few minutes. Let it finish. Use the VS Code terminal to run `make`.

## 2. First-time git setup (once per computer)

Run these in Ubuntu, with straight quotes and your real details:

```
git config --global user.name "Your Name"
git config --global user.email "your-github-email@example.com"
```

1. Create a GitHub account if you do not have one and accept the repository
   invite (it arrives by email, or open
   https://github.com/AsiyaKondusa/seasonal-forest/invitations).
2. GitHub does not accept passwords for git. Make a **personal access token**:
   GitHub > profile picture > Settings > Developer settings > Personal access
   tokens > Tokens (classic) > Generate new token (classic). Name it, set an
   expiry of at least 30 days, tick **repo**, generate, and copy it now
   (it starts with `ghp_` and is shown only once).
3. Let git remember it:
   `git config --global credential.helper store`
   The first time git asks for a password, paste the token (nothing shows while
   you paste, then press Enter).

## 3. Get the code, build and run

Clone it inside your Linux home folder (not under `/mnt/c`):

```
cd ~
git clone https://github.com/AsiyaKondusa/seasonal-forest.git
cd seasonal-forest
make
./forest
```

You should see a terrain grid with trees and a slowly orbiting camera.
Close the window to quit. Always run `./forest` from inside the project folder,
because it looks for `trunk.obj` and `canopy.obj` in the current folder.

`make` builds, `make run` builds and runs, `make clean` deletes the program.

## 4. How we work with git (every task, every time)

Nobody commits straight to `main`. Everyone works on their own branch
(names are in `TASKS.md`).

**Start**

1. `cd ~/seasonal-forest`
2. `git checkout main`
3. `git pull`
4. Create and switch to your branch (first time only):
   `git checkout -b your-branch-name`
   Next time, just switch to it: `git checkout your-branch-name`

**While you work**

5. Edit, then build and test: `make` and `./forest`.
6. Look at what changed: `git status`. Never commit the `forest` program.
7. Save a commit (do this often, small and with a clear message):
   ```
   git add .
   git commit -m "Describe what you did"
   ```
8. Send it to GitHub. The first time on a branch:
   `git push -u origin your-branch-name`
   After that, just `git push`.

**Hand your work in (Pull Request)**

9. Open https://github.com/AsiyaKondusa/seasonal-forest. A yellow banner shows
   "Compare & pull request" for your branch. Click it, check that the base is
   `main`, write what you changed and how you tested it, then click
   **Create pull request**.
10. Ask a teammate to review: they open the **Files changed** tab, and ideally
    check out your branch and run it. When it works they click
    **Merge pull request** then **Confirm merge**.
11. After a merge, everyone updates:
    ```
    git checkout main
    git pull
    git checkout your-branch-name
    git merge main
    ```
    Do this before continuing work on your own branch.

## 5. Merge conflicts

A conflict means two people edited the same lines. Git stops and marks the file
with `<<<<<<<`, `=======` and `>>>>>>>`. Do not panic and do not delete the
folder.

1. Open the file and read both versions between the markers.
2. Keep the correct combination, and delete the three marker lines.
3. `make` and test that it still runs.
4. `git add .` then `git commit -m "Resolve merge conflict"` then `git push`.

If you are not sure which version is right, ask in the group chat before
deciding.

## 6. Rules

- Never push directly to `main`.
- Never use `git push --force`.
- Do not commit compiled programs, `.o` files, `.blend1` backups, or videos
  over a few MB. Share large videos by Drive link.
- Put new work in your own files. Keep edits to `main.cpp` small and tell the
  group when you change it.
- Commit small and often. Pull `main` before you start each session.

## 7. If something goes wrong

| Problem | Fix |
|---|---|
| `make: command not found` or `g++: command not found` | `sudo apt install build-essential` |
| `SDL2/SDL.h: No such file` | `sudo apt install libsdl2-dev` |
| `Makefile: missing separator` | The recipe lines must start with a Tab, not spaces |
| Window does not open on Windows | `echo $DISPLAY` must print something; run `wsl --update`, restart |
| "Could not load .obj files" | Run `./forest` from inside `seasonal-forest` |
| `Authentication failed` on push | Paste the personal access token, not your password |
| Push rejected because remote has new work | `git pull`, resolve conflicts if any, then `git push` |
| Committed the wrong file | Ask before trying to fix it, so history is not damaged |

## 8. Project files

- `main.cpp`: window, main loop, projection, drawing, OBJ loader, camera
- `mat4.h`: Mat4 and the transforms (translation, scaling, rotationY)
- `trunk.obj`, `canopy.obj`: tree models (canopy base sits at y = 1)
- `Makefile`: build rules
- `models/`: Blender source files (`.blend`), added by Person B
- `blender/`: Blender timelapse scene and scripts, added by Person C
- `experiments/`: early test programs, not part of the build
- `README.md`: this file. `TASKS.md`: remaining work and owners

## 9. Technical notes for the next phases

- Matrices are `m[row][col]` with translation in the last column and the
  column-vector convention, so `A * B * v` applies B first. OpenGL is
  column-major, so transpose a `Mat4` before `glLoadMatrixf`.
- `gView` is the camera matrix, set every frame in `main()`, and `toView()`
  applies it to a point.
- `Mesh` stores vertices and edges only. Faces are not kept yet, so the loader
  must be extended for filled polygons.
- `drawEdge3D` skips an edge if either end is behind the near plane (z < 0.1).
- `canopyMatrix(t, s)` scales a canopy around its base at y = 1. The scale `s`
  currently pulses with a sine wave for testing and should come from the season.
- OBJ face numbers start at 1, and the loader subtracts 1.
- The wireframe renderer draws in software, so keep models low-poly.
  Triangulated models show their diagonals in wireframe, which is expected.