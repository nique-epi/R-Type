# R-Type

A networked multiplayer remake of the R-Type shoot'em up, built on a custom C++ game engine.

The repository currently holds the build system and empty client and server entry points.

The code is split into engine, game and network libraries; see [docs/architecture.md](docs/architecture.md).

## Requirements

- CMake 3.28 or newer
- A C++20 compiler: GCC or Clang on Linux, MSVC (Visual Studio 2022) on Windows
- Git

Third-party libraries (SFML, Asio, GoogleTest) are built by [vcpkg](https://vcpkg.io), pinned as a Git submodule and bootstrapped automatically on the first configure.

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

The first configure builds every dependency and takes a few minutes. The `r-type_server` and `r-type_client` binaries are written at the repository root, or in `Release/` with the Visual Studio generator.

If the repository was cloned without its submodules, run `git submodule update --init` first.

## Tests

```bash
cmake --workflow --preset test
```

## Code style

The `clang-format` and `clang-tidy` configurations live at the repository root. After a configure, these targets are available:

| Target | Effect |
|---|---|
| `format` | Formats every source file in place |
| `format-check` | Fails if a source file is not formatted |
| `tidy` | Runs `clang-tidy` on every source file |

```bash
cmake --build build --target format
```
