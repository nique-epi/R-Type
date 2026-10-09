# R-Type

A networked multiplayer remake of the R-Type shoot'em up, built on a custom C++ game engine.

The client opens an empty window. The server listens for UDP datagrams on port 4242 and logs each one at the `debug` level; it does not answer them yet.

The code is split into engine, game and network libraries; see [the architecture decision](docs/src/content/docs/architecture.md).

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

The client reads its files from `assets/`, next to its executable or in the folder above it, wherever it is launched from. It stops with exit code 1, naming both folders, when it finds neither.

If the repository was cloned without its submodules, run `git submodule update --init` first.

## Tests

```bash
cmake --workflow --preset test
```

## Logging

The client and the server write a timestamped journal, `r-type_server.log` or `r-type_client.log` in the current directory. The level and the output are chosen at launch:

```bash
./r-type_server --log-stderr --log-level debug
```

| Option | Effect |
|---|---|
| `--log-level <name>` | `trace`, `debug`, `info` (default), `warn`, `error` or `silent` |
| `--log-file <path>` | Write the journal to this file |
| `--no-log-file` | Do not write a journal |
| `--log-stderr` | Also write to standard error |

The build option `-DLOG_LEVEL=<name>` removes the calls below a level from the binaries. See [the logging page](docs/src/content/docs/logging.md).

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

## Documentation

The documentation site is built with [Starlight](https://starlight.astro.build) and published at <https://nique-epi.github.io/R-Type/>. Each page is a Markdown file in `docs/src/content/docs/`.

To preview it locally, with Node.js 22.12 or newer:

```bash
cd docs
npm ci
npm run dev
```

`npm run build` fails on a broken internal link, like the `docs` check on pull requests.
