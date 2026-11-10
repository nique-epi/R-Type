---
title: Build from source
description: The requirements, the build command on Linux, macOS and Windows, the optional modules and the packages.
sidebar:
  label: Build from source
  order: 2
---

R-Type builds with CMake presets. vcpkg, pinned as a Git submodule, builds every third-party library on the first configure, so the only things to install are a compiler, CMake and Git.

## Requirements

| Tool | Version |
|---|---|
| CMake | 4.2 or newer |
| C++ compiler | C++20: GCC or Clang on Linux, Apple Clang on macOS (Apple Silicon), MSVC (Visual Studio 2022) on Windows |
| Git | any recent version |

SFML, Asio, toml++, SQLiteCpp, Luau and GoogleTest come from vcpkg; nothing else is installed by hand.

On Linux, SFML and vcpkg need a few system packages. On Ubuntu:

```bash
sudo apt-get install build-essential cmake git curl zip unzip tar pkg-config libx11-dev libxi-dev libxrandr-dev libxcursor-dev libudev-dev libgl1-mesa-dev
```

## Build

```bash
git clone --recurse-submodules git@github.com:nique-epi/R-Type.git
cd R-Type
cmake --workflow --preset build
```

The `r-type_server` and `r-type_client` binaries are written at the root of the repository.

:::note[The first configure takes a few minutes]
vcpkg builds every dependency once. The next builds only compile what changed.
:::

:::tip[Cloned without the submodules?]
Run `git submodule update --init` once, then the build command again.
:::

### Windows

Run the same commands in a **Developer PowerShell for Visual Studio 2022**, with the "Desktop development with C++" workload installed. The binaries are then `Release\r-type_server.exe` and `Release\r-type_client.exe`.

### Where the programs find their files

The client and the server read their files from `assets/`, looked for next to their executable, in the folder above it, or in `Resources/` beside it for the macOS app, wherever they are launched from.

:::caution
A program that finds no `assets/` folder stops with exit code 1 and names the three folders it looked in.
:::

## Optional modules

Each optional module has a CMake option, on by default. Turned off, its folder is not compiled and the program starts without it:

| Option | Leaves out |
|---|---|
| `RTYPE_MODULE_AUDIO` | the sound of the client |
| `RTYPE_MODULE_METRICS` | the metrics of the client and of each game process |
| `RTYPE_MODULE_DEBUG` | the lagometer of the client |
| `RTYPE_MODULE_MASTER_LINK` | the registration of the server with the master |
| `RTYPE_MODULE_ADMIN_CONSOLE` | the administration console of the supervisor |
| `RTYPE_MODULE_CHAT` | the chat of the games |
| `RTYPE_MODULE_PLUGINS` | the loading of plugins |
| `RTYPE_MODULE_GAME_STATISTICS` | the `game-statistics` plugin; off whenever `RTYPE_MODULE_PLUGINS` is |

```bash
cmake --preset default -DRTYPE_MODULE_AUDIO=OFF
cmake --build build --parallel
```

The configure ends with the list of the modules built and left out. What each module does is on [Modules](/R-Type/modules/).

## Packages

```bash
cmake --workflow --preset package
```

This builds in Release, then writes one set of packages for the client and one for the server in `build/`:

| System | Client | Server |
|---|---|---|
| Linux | `.tar.gz`, `.AppImage` | `.tar.gz` |
| Windows | `.zip`, `.exe` installer | `.zip`, `.exe` installer |
| macOS | `.dmg` holding `R-Type.app` | `.tar.gz` |

Each package holds its program and the part of `assets/` it reads, so it starts from any folder.

:::note[Packaging tools]
The installer and the AppImage of the client, and the updates the client installs (see [Client](/R-Type/client/#updates-clientupdatemodule-releasefeedsource)), are made by `vpk`, the tool of Velopack, a .NET tool: `dotnet tool install -g vpk --version 1.2.161`. Without it the client package is made with no installer and no AppImage, and cannot update itself. On Windows, the installer of the server needs [NSIS](https://nsis.sourceforge.io).
:::

The CI builds the packages of the three systems on every push to `main`. Pushing a tag `v<version>` that matches the version of `project()` in `CMakeLists.txt` publishes them as a GitHub release, which [rtype.app/download](https://rtype.app/download) then offers to players. Signing and notarization on macOS are described in the README of the repository.
