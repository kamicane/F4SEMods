# F4SE plugins

This repository is a monorepo that houses multiple Fallout 4 mod projects and shared libraries used across them.

The purpose is to keep the build infrastructure and related / shared code together for simplified dependency management.

For the time being, this build system will support both Fallout 4 1.10.163 and Anniversary Edition.

## CommonLibF4

This build system includes two versions of CommonLibF4: One for AE and one for pre-NG.
They are submodules in ./extern, pulled from my fork of [powerof3/CommonLibF4](https://github.com/powerof3/CommonLibF4). Both submodules point to the same repo at different branches.

The [more up-to-date CommonLibF4 version](https://github.com/libxse/commonlibf4) is unfortunately incompatible with pre-NG versions, has a completely different build system and rewritten api. I chose to go with powerof3's CommonLibF4 as a base because CMake support was never dropped. It allows me to support two versions at the same time with (not so) minimal effort.

There are only minimal changes and and "backported" ids (from libxse) in [kamicane/CommonLibF4](https://github.com/kamicane/CommonLibF4) that is used in this repo. Every other hack will remain project specific.

## Layout

- `Projects/PROJECT_ID/`: separate projects
- `extern/`: CommonLibF4 libraries
- `include/`: shared headers used across projects
- `Interface/`: shared interface files, using decompiled actionscript sources from [F4CF/Interface](https://github.com/F4CF/Interface) for building

Building is done with CMake presets. It switches what ./src/main.cpp pulls in. Each subproject provides a project.hpp file with a common export interface so that main.cpp can read it. Projects do not provide cpp files, only headers.

vcpkg is used for external package management (see `vcpkg.json`)

Due to CommonLibF4's CMake files, the fmt and spdlog dependencies are dictated by CommonLibF4. I could not modify their build options.

## Code server

I use vscode with the clangd extension. This repo assumes the correct `compile_commands.json` for the desired project is copied in the root (the cmake vscode extension can do that). Clangd will require a restart after every preset switch.

clangd works mostly fine, but might crash if you open one too many CommonLibF4 headers.

cpptools is just too slow to be viable, especially with this setup.

## Building

Building is done with msvc. This means vscode will have to be opened with the correct environment.
