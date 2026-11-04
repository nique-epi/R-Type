# R-Type

A networked multiplayer remake of the R-Type shoot'em up, built on a custom C++ game engine.

The client plays its theme in a loop, then the music of a game a second after a game starts, the sounds of the intro and of the game, and a click on the buttons of its menus. It opens on a second of black and an intro of 5.5 s set to the beats of the theme, which a key or a click skips, then on its title screen, over a scrolling starfield: PLAY opens the connection screen, then the game, RANKING opens the best scores of a server, SETTINGS opens the options (sound, keys, display mode and window size, accessibility), QUIT asks before closing the client; Escape during a game opens the options over it. The server listens for UDP datagrams on port 4242, or on the port given at launch: it answers each connection request with a challenge, an acceptance with a player number from 1 to 4, or a refusal when the game is full, and logs the datagrams of the players it admitted at the `debug` level.

The code is split into engine, game and network libraries; see [the architecture decision](docs/src/content/docs/architecture.md).

## Requirements

- CMake 4.2 or newer
- A C++20 compiler: GCC or Clang on Linux, MSVC (Visual Studio 2022) on Windows
- Git

Third-party libraries (SFML, Asio, toml++, SQLiteCpp, GoogleTest) are built by [vcpkg](https://vcpkg.io), pinned as a Git submodule and bootstrapped automatically on the first configure.

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

The client and the server read their files from `assets/`, next to their executable or in the folder above it, wherever they are launched from. Each stops with exit code 1, naming both folders, when it finds neither.

If the repository was cloned without its submodules, run `git submodule update --init` first.

### Packages

```bash
cmake --workflow --preset package
```

This builds in Release, then writes in `build/` one set of packages for the client and one for the server, so a player installs the client alone and a host the server alone:

| System | Client | Server |
|---|---|---|
| Linux | `R-Type-client-<version>-Linux.tar.gz`, `R-Type-client-<version>-Linux.AppImage` | `R-Type-server-<version>-Linux.tar.gz` |
| Windows | `R-Type-client-<version>-win64.zip`, `R-Type-client-<version>-win64.exe` (installer) | `R-Type-server-<version>-win64.zip`, `R-Type-server-<version>-win64.exe` (installer) |
| macOS | `R-Type-client-<version>-Darwin.dmg`, holding `R-Type.app` | `R-Type-server-<version>-Darwin.tar.gz` |

Each package holds its program and the part of `assets/` it reads, in the folder above the executable (`bin/` and `assets/`; `usr/bin/` and `usr/assets/` in the AppImage; `Contents/MacOS/` and `Contents/assets/` in the app), so it starts from any folder. The version is the one of `project()` in `CMakeLists.txt`. The files that build the packages are in `packaging/`.

- Linux: the programs need X11, OpenGL and udev from the distribution, and a glibc at least as recent as the one of the machine that built them (Ubuntu 24.04 on the CI). The AppImage needs FUSE, which most desktops have. Packaging the client on Linux needs [`appimagetool`](https://github.com/AppImage/appimagetool) on the `PATH`.
- Windows: the installer of each program puts it in its own folder (`R-Type`, `R-Type Server`) with a Start menu shortcut and an uninstaller. Packaging needs [NSIS](https://nsis.sourceforge.io).
- macOS: the app is built for Apple Silicon only and is not notarized: the first time, open it with right-click > Open.

The CI builds the packages of the three systems on every pull request and keeps them 7 days as artifacts of the run (`packages-Linux`, `packages-Windows`, `packages-macOS`). Pushing a tag `v<version>` that matches the version of `project()` publishes them in a GitHub release, with notes listing the pull requests merged since the previous tag:

```bash
git tag v0.1.0
```

```bash
git push origin v0.1.0
```

The release refuses a tag whose version is not the one of `project()`; change the version in a pull request first. `assets/README.md` says the licence of the Epidemic Sound files is not confirmed for a public release.

## Run

```bash
./r-type_server -p 4243
```

The server listens on UDP port 4243 of every IPv4 interface; without `-p`, on port 4242.

```bash
./r-type_server --seed 42
```

The Bydos appear at heights drawn from the seed; without `--seed` the server draws one, and the journal records it (`Bydo spawning starts from the seed ...`) so the same game can be replayed.

```bash
./r-type_server --set state-rate=30
```

`--set <name>=<value>` forces any setting of the server and wins over `server.cfg`, a file of commands the server reads from the folder it is started from. A missing file is fine; the defaults apply. `server.cfg.example`, at the root of the repository, lists every setting with its default and its bounds: copy it to `server.cfg`. Every command and setting is described in [Configuration commands](https://nique-epi.github.io/R-Type/commands/).

```
# server.cfg
set port 4300
module lobby off
```

```bash
./r-type_server --state-rate 30
```

Every player's input is held for the same few ticks before it moves the ship (3 ticks, 50 ms, by default); `--input-delay` changes it, from 0 to 30 ticks.

```bash
./r-type_server --input-delay 5
```

The server sends each player 30 world states per second instead of 20, from 2 to 60: more states move the other ships more smoothly and use more bandwidth (about 350 bytes per state with four players and thirty entities, see [the server page](https://nique-epi.github.io/R-Type/server/)).

The server reads the waves of Bydos from `assets/waves/waves.toml`, in the `assets/` folder it finds next to its executable or in the folder above it. A missing or invalid file stops it with exit code 1, naming the file, the line and the key at fault; the format is described in [Writing the waves file](https://nique-epi.github.io/R-Type/waves-file/).

```bash
./r-type_client -h 192.168.1.20 -p 4243 -n ACE
```

The client is given the IPv4 address and the port of the server, `127.0.0.1` and 4242 by default, and the nickname of the player, `PILOT` by default; its connection screen starts with them. CONNECT on that screen joins the server, so start the server first: without one, the screen shows `SERVER UNREACHABLE` after 5 seconds.

`--no-sound` starts the client without sound: the screens and the volumes work as usual, and nothing plays.

```bash
./r-type_client --network bad
```

The client's datagrams cross a simulated bad network: 300 ms more ping, 10 % of the datagrams lost, some doubled or out of order. Both programs take `--network` with `4g`, `wifi-busy` or `bad`, and `--lag`, `--jitter`, `--loss`, `--duplicates` and `--reorder` to set one value; the journal warns that the network is simulated (see [the network page](https://nique-epi.github.io/R-Type/network/)).

Without these options, `tests/tools/network-conditions/` applies the same scenarios to the loopback interface from the system: `sudo tests/tools/network-conditions/netem.sh apply --network bad` on Linux, `dummynet.sh` on macOS, then `clear` to remove them (see [the network page](https://nique-epi.github.io/R-Type/network/#network-conditions-from-the-system-teststoolsnetwork-conditions)).

```bash
./r-type_server --traffic-csv traffic.csv
```

Both programs write what crosses their socket to the journal every 10 s; with `--traffic-csv` they also write it to a file every second, by player and by message type, for a measurement campaign (see [the network page](https://nique-epi.github.io/R-Type/network/#traffic-counters-trafficcounters)).

```bash
./r-type_server --mode master -p 8080
```

The same binary runs the master in this mode: the program that will list the game servers for the players and take the orders of an admin. `--mode` comes before every other option. Today it answers an HTTP API under `/api` on TCP port 8080 of every IPv4 interface (`curl http://127.0.0.1:8080/api/health` answers `{"version":"0.1.0"}`), keeps its data in `r-type_master.db` next to the executable, and reads `master.cfg`, modelled by `master.cfg.example` (see [the master page](https://nique-epi.github.io/R-Type/master/)).

- **Why HTTP:** the subject requires UDP for every message of a game and accepts TCP for other needs when it is justified. The master carries no message of a game, and the web administration the subject accepts needs HTTP in a browser, so one protocol serves the programs and the web site, readable with `curl`. A player still joins a server over UDP when the master is stopped.
- **Why a mode and not a third binary:** the subject lists two binaries, `r-type_server` and `r-type_client`. The master ships inside the first and reuses its stop on Ctrl-C, its configuration file and its journal; it still runs as a process of its own, so a master that stops stops no game.

Both choices are told in full in [Architecture](https://nique-epi.github.io/R-Type/architecture/#master).

`--help` prints the options of either program, and `--mode master --help` those of the master. An invalid argument stops it with a message and exit code 1.

## Tests

```bash
cmake --workflow --preset test
```

```bash
./build/tests/malformed_datagrams --server 4242
```

Once the test preset is built, this sends the server running on port 4242 of the same machine malformed datagrams for one minute, then checks that it still answers a connection request. `--client <port>` does the same to a client; see [Network](https://nique-epi.github.io/R-Type/network/#malformed-datagrams-malformed_datagrams).

```bash
./build/tests/load_test_bots --bots 32 --seconds 60
```

This has 32 players with no window join the server on port 4242 of the same machine, fill its eight games and play them for one minute, then prints the world states each one received per second; the server's journal gives the duration of its ticks every 10 s. See [Network](https://nique-epi.github.io/R-Type/network/#load-test-bots-load_test_bots).

## Logging

The client, the server and the master write a timestamped journal, `r-type_client.log`, `r-type_server.log` or `r-type_master.log` next to their executable, and show the same lines in the terminal; the first line gives the absolute path of the journal. Installed in a folder they cannot write to, they write no journal and say so in the terminal. The level and the output are chosen at launch:

```bash
./r-type_server --log-level debug
```

| Option | Effect |
|---|---|
| `--log-level <name>` | `trace`, `debug`, `info` (default), `warn`, `error` or `silent` |
| `--log-file <path>` | Write the journal to this file |
| `--no-log-file` | Do not write a journal |
| `--log-stderr` | Write to standard error too (default) |
| `--no-log-stderr` | Do not write to standard error |

The build option `-DLOG_LEVEL=<name>` removes the calls below a level from the binaries. See [the logging page](docs/src/content/docs/logging.md).

The client reads `client.cfg` the same way, and `client.cfg.example` is its model (`set nickname`, `set server-address`, `set server-port`, `module <name> on|off`, and `--set` to force a setting). It keeps the player's settings in `settings.ini`: in `$XDG_CONFIG_HOME/r-type` (or `~/.config/r-type`) on Linux, `~/Library/Application Support/R-Type` on macOS, `%LOCALAPPDATA%\R-Type` on Windows. Its path is logged at launch; deleting it brings back the settings of a first launch.

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

The documentation site is built with [Starlight](https://starlight.astro.build) and published at <https://nique-epi.github.io/R-Type/>. Each page is a Markdown file in `docs/src/content/docs/`. To add a module to the server or the client, start with the [Modules](https://nique-epi.github.io/R-Type/modules/) page and the [tutorial](https://nique-epi.github.io/R-Type/tutorial-write-a-module/). Two more tutorials [add a network message](https://nique-epi.github.io/R-Type/tutorial-add-a-network-message/) and [run a server with several games](https://nique-epi.github.io/R-Type/tutorial-run-several-games/).

To preview it locally, with Node.js 22.12 or newer:

```bash
cd docs
npm ci
npm run dev
```

`npm run build` fails on a broken internal link, like the `docs` check on pull requests.

## Contributing

How to branch, write a commit, write and test the code, open a pull request and know when a change is done is summed up in [the contributing guide](CONTRIBUTING.md).
