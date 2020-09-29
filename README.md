# Engine

The repo is a home of a game that are developing mostly for an Android Platform;
It's a simple 2D puzzle game that is a demo of an Engine and an actual reason of this repo;

## Info
Game's engine has a hybrid component based architecture with an OpenGL rendering backend;

Supported platforms are:
 - Linux
 - Android
 - Windows

## Building
The Engine build system based on Cmake scripts & Tacos build framework; A compiler with at least C++17 support required to successfully perform a build;

The next command will build Engine, Game, Tests and all 3rd party dependicies for a current platform, also it will execute all provided unittestes and content tests;

```
> python Sources\Libs\Tacos\tacos.py --project=Automation\BuildGame.json

# Build artifact will be saved to a '_build' folder

```

## Depedicies
There is list of 3rd party dependecies used in Engine

- GLFW
- GLAD
- Oboe
- GTest
- RapidJSON
- Taccos
- OpenAL-Soft
- FreeType2
- ZLIB
- STB

# Editor

The Engine's Editor is PyQt5 application that allow to setup/update game entities; Editor is communicating with Engine via ctype API;

## Info

The next command will start an Editor

```
> python Sources\Editor\App\Editor.py

```
