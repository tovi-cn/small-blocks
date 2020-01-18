# Small Blocks

Small Blocks is a 3D building game where the player can shrink and grow to build creations of any size.
You can build with extremely large blocks, extremely small blocks, or why not something in between.

![](Small_Blocks.gif)

## Download

[Windows 64-bit](https://github.com/carlenlund/small-blocks/releases/download/v0.1.0/small-blocks-v0.1.0.zip)

Also install Microsoft's [Visual C++ 2019 Redistributable](https://aka.ms/vs/16/release/VC_redist.x64.exe) if you haven't already.
This makes sure that the program starts correctly with all necessary dependencies.

See build instructions below for usage on other platforms.


## Controls

- Move with mouse and `W` `A` `S` `D`
- Fly with `SPACE`
- Left click to break block
- Right click to place block
- Select block color with `1` - `5`
- Shrink player with `Q` and grow player with `E` (or use scroll wheel)

Other controls:

- Middle click to copy block
- Run with `SHIFT`
- Shrink and grow block independently of player size with `Z` and `C`
- Regenerate world with `R`
- Toggle wireframe mode with `G`

## Compiling

### macOS and Linux

On macOS and Linux you can use the command-line tools CMake and Make to compile the game.

Compile the game by running `cmake .` followed by `make`.

### Windows

On Windows you can use Visual Studio to compile the game.

To compile with Visual Studio, open the project as a "local folder",
then select `small-blocks.exe` as the build target, and lastly build the project.

To make sure the executable runs in the correct directory, go to "Debug and Launch Settings"
and add the following line under `configurations`:

```
"currentDir": "${workspaceRoot}"
```
