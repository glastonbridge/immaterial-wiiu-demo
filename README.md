# Slipstream's Revision Wii-U demo

Slowly evolving from https://github.com/vurpo/wiiu_example and https://github.com/devkitPro/wut

All original source is unlicensed. Our legal team is confused. The people working on this project should probably agree on what we're going to do with this source after the demo is released. If the entire dev team mysteriously vanishes, all copyright ownership will automatically transfer to Crypt's cat.

## Developer setup - Emulated

Version numbers and installation steps are just my notes about what worked for me, that might help other coders get set up.

### Install Cemu

Cemu is a Wii-U emulator

1. Go here, install https://github.com/cemu-project/Cemu/releases/tag/v2.0-61
2. Take a note of the path to the `sdcard/` directory, you will need it when you do the `cmake` step.

If anyone figures out how to do debugging with this, please update this README

### Install the CafeGLSL library for GLSL shaders to work

1. Grab `glslcompiler.rpl` from https://github.com/Exzap/CafeGLSL/releases/tag/v0.1.1
2. In the directory that contains `Cemu.exe` create a new subdirectory called `cafeLibs`
3. Put `glslcompiler.rpl` in it

### Install devKitPro and WUT

DevKitPro is a homebrew development environment for a few platforms. It has its own version of the pacman package manager.

WUT is the Wii-U toolchain that works in DevKitPro.

1. Go here, install https://devkitpro.org/wiki/Getting_Started
2. (windows-only) Once installed, open the msys2 prompt inside the installed toolkit `devkitPro\msys2\msys2.exe`
3. As per the instructions [for WUT](https://github.com/devkitPro/wut), run `pacman -Syu --needed wiiu-dev`

## Developer setup - Real hardware

### Install EnvironmentLoader and Aroma

Follow https://wiiu.hacks.guide and then https://aroma.foryour.cafe , then start your Wii U in the Aroma environment. Aroma also has some optional plugins you can install, such as an FTP server and some debugging tools (TBD how to debug).

### Installing and running

On your SD card, place `glslcompiler.rpl` in `/wiiu/libs/` and `wiiu_demo.rpx` in `/wiiu/apps/wiiu_demo/`.

The `assets` and `shaders` directories go in the root of the SD card, unless we decide to store those somewhere that makes more sense.

`wiiu_demo.rpx` will appear as an app you can launch on the Wii U home screen. Use the `udplogserver` command that came with wut to view the logs of the app in realtime.

### Debugging

TBD

## Building and running

Assumes you're using the `msys2.exe` command prompt mentioned above.

### Making your makefiles using cmake

You need to do this every time the files in the project change. 
This isn't normal cmake, it's been blessed with the right compiler settings.

1. `cd wiiu-demo`
2. (Optional) edit `CmakeLists.txt` to include your new files
2. `mkdir build`
3. `cd build`
4. `/opt/devkitpro/portlibs/wiiu/bin/powerpc-eabi-cmake .. -DCMAKE_INSTALL_PREFIX=[path to the sdcard]`
5. Optionally, also pass `-DCEMU_EXECUTABLE=[path to cemu] -DSDCARD_DIR=[path to SD card directory to run executable from, defaults to install dir]` to set up `make test` to compile, install and run the emulator. If you are on windows, make the `SDCARD_DIR` a Windows-side path but with forward slashes.
6. For sync: Pass -DSYNC_IP=your.ip.here.please to set the IP the synctool should connect to (you can in theory use this to sync on-device [untested!]. defaults to localhost, which will work with the emulator.), and set -DSYNC_PLAYER to "false" to make the demo actually connect to the synctool (by default it plays the tracks from the SD card, as it would in the final build).
Note that in step 4, the path must be posix, in the way that `msys2.exe` structures paths. That usually means that a path in your `C:\` filesystem is translated to `/c/` and you use forward slashes (as god intended).
7. For wiiload send, there is a `wiiload` target that sends the data to `-DWIILOAD_IP=[your wiiu ip]`. Warning: Does NOT copy assets. Put those on the root of your sdcard yourself!

### HOW 2 SYNC

Just put syncVal("PartName:ParameterName") anywhere you want a float from the synctool, the rest is automatic (maybe don't do it in an inner loop though). If you use PartName:ParamName syntax, things will be grouped by PartName in the slightly nicer rocket tool emoon made. 

### Building your code!

1. `cd wiiu-demo/build`
2. `make`
3. `make install` - install assets to the sd card

### Running

1. Open Cemu
2. Click `Load...`
3. Navigate to `wiiu-demo/build/wiiu_demo.rpx` and open it
4. Congratulations, you have made a demo!

## Project structure

```
main.cpp -- bootstrap a renderer and load a scene
shaders/ -- shader resource files
renderer/ -- GX2 code, converts scene data to visual things
```