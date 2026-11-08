# R-Type

A networked multiplayer remake of the R-Type shoot'em up, built on a custom C++ game engine. Up to four pilots fly together in a game against waves of Bydos; the server owns the game, the client draws it and sends the keys held. One server runs up to eight games at once. Epitech tek3 project.

- **Platforms:** Linux (GCC or Clang), Windows (MSVC, Visual Studio 2022) and macOS (Apple Silicon). The CI builds and tests Linux and Windows on every pull request.
- **Language and libraries:** C++20, SFML 3 on the client, standalone Asio on the server, GoogleTest for the tests.
- **Programs:** `r-type_client` (window, sound, menus) and `r-type_server` (UDP on port 4242 by default). By default it is a supervisor that starts one process per game and sends each player to the game that has a place; `--mode server` hosts the games in one process, and `--mode master` runs the master, the program that lists game servers.
- **Network:** every message of a game travels over UDP; a client that lacks the sheets of a server, a modded one for instance, downloads its content pack over HTTP first. The protocol is specified byte by byte in [the protocol RFC](https://nique-epi.github.io/R-Type/protocol/).

The code is split into engine, game and network libraries; see [the architecture decision](docs/src/content/docs/architecture.md) and [the project layout](docs/src/content/docs/project-layout.md).

## Quick start

```bash
git clone --recurse-submodules git@github.com:nique-epi/R-Type.git
cd R-Type
cmake --workflow --preset build
./r-type_server
./r-type_client
```

Run the last two commands in two terminals. In the client, PLAY opens the server map, which needs the master; to join a server directly, launch the client with `-h <address>`, so PLAY joins it at once, or open the connection screen with D (ADDRESS) and press CONNECT. The sections below give the prerequisites for each system, every option, and a game of four.

### A game of four players

On the machine that hosts (its address on the network is `192.168.1.20` here):

```bash
./r-type_server -p 4242
```

On each of the four machines, or four terminals of one machine, with a different nickname:

```bash
./r-type_client -h 192.168.1.20 -p 4242 -n ACE
./r-type_client -h 192.168.1.20 -p 4242 -n BLAZE
./r-type_client -h 192.168.1.20 -p 4242 -n COMET
./r-type_client -h 192.168.1.20 -p 4242 -n DASH
```

Each client is given its server with `-h`, so PLAY on the title joins it at once, its connection screen filled with these values; without `-h`, PLAY opens the server map, and D (ADDRESS) there opens the connection screen, where CONNECT joins. The supervisor sends the four pilots to the same game, where each gets a number from 1 to 4. A fifth player lands in a second game, and is refused only when the eight games are full. Once every connected pilot says it is ready in the waiting room, the game starts; the arrows move the ship and the space bar shoots, until the player changes the keys in SETTINGS. When it ends, the end screen ranks the four pilots.

## Features

The client opens on a second of black and an intro set to the beats of its theme, which a key or a click skips, then on its title screen over a scrolling starfield: PLAY opens the server map, then the game, RANKING opens the best scores of a server, SETTINGS opens the options (sound, keys, display mode and window size, accessibility), QUIT asks before closing the client; the hangar, opened from the server map or from the own bay of the waiting room, dresses the ship with one of ten items, set on it by robotic arms; Escape during a game opens the options over it. It plays a theme, the sounds of the intro and of the game, and a click on its menus. The server answers each connection request with a challenge, an acceptance with a player number from 1 to 4, or a refusal when the game is full, and logs the datagrams of the players it admitted at the `debug` level.

## Requirements

- CMake 4.2 or newer
- A C++20 compiler: GCC or Clang on Linux, MSVC (Visual Studio 2022) on Windows
- Git
- For the web site only: bun 1.3 and Node.js 22.12 or newer

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

On Windows, run the same commands in a Developer PowerShell for Visual Studio 2022 (the "Desktop development with C++" workload); the binaries are then `Release\r-type_server.exe` and `Release\r-type_client.exe`, and `.\Release\r-type_server.exe` starts a server.

The first configure builds every dependency and takes a few minutes. The `r-type_server` and `r-type_client` binaries are written at the repository root, or in `Release/` with the Visual Studio generator.

The client and the server read their files from `assets/`, next to their executable, in the folder above it, or in `Resources/` beside it for the macOS app, wherever they are launched from. Each stops with exit code 1, naming the three folders, when it finds none.

If the repository was cloned without its submodules, run `git submodule update --init` first.

### Build options

Each optional module has a CMake option, on by default. Off, its folder is not compiled, its `Contract/` excepted, the list of its program does not name it, and the program starts without it:

| Option | Leaves out | Without it |
|---|---|---|
| `RTYPE_MODULE_AUDIO` | the sound of the client | the client plays nothing, and `--no-sound` changes nothing |
| `RTYPE_MODULE_METRICS` | the metrics of the client and of each game process | no report in the journal, and `--traffic-csv` writes no file |
| `RTYPE_MODULE_MASTER_LINK` | the registration of the server with the master | the server never registers, and `set master-url`, `set master-key` and `set public-address` are refused |
| `RTYPE_MODULE_ADMIN_CONSOLE` | the administration console of the supervisor | the supervisor reads nothing from its terminal, and `games`, `players`, `close-game`, `network` and `help` are unknown commands |
| `RTYPE_MODULE_CHAT` | the chat of the games | a chat line reaches nobody |
| `RTYPE_MODULE_PLUGINS` | the loading of plugins | `plugin` and `plugin-folder` are unknown commands |
| `RTYPE_MODULE_GAME_STATISTICS` | the `game-statistics` plugin; off whenever `RTYPE_MODULE_PLUGINS` is | no `game-statistics` file in `plugins/` |

```bash
cmake --preset default -DRTYPE_MODULE_AUDIO=OFF
```

The configure ends with the list of the modules built and left out; `cmake --build build --parallel` then builds them. A line of `server.cfg` refused for a module left out is written to the journal at the `warn` level, and the program starts all the same.

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

Each package holds its program and the part of `assets/` it reads, where its executable finds them (`bin/` and `assets/`; `usr/bin/` and `usr/assets/` in the AppImage; `Contents/MacOS/` and `Contents/Resources/assets/` in the app, where Apple wants every file that is not code), so it starts from any folder. The version is the one of `project()` in `CMakeLists.txt`. The files that build the packages are in `packaging/`.

- Linux: the programs need X11, OpenGL and udev from the distribution, and a glibc at least as recent as the one of the machine that built them (Ubuntu 24.04 on the CI). The AppImage needs FUSE, which most desktops have. Packaging the client on Linux needs [`appimagetool`](https://github.com/AppImage/appimagetool) on the `PATH`.
- Windows: the installer of each program puts it in its own folder (`R-Type`, `R-Type Server`) with a Start menu shortcut and an uninstaller. Packaging needs [NSIS](https://nsis.sourceforge.io).
- macOS: the app is built for Apple Silicon only, with the hardened runtime. When `RTYPE_MACOS_SIGNING_IDENTITY` names a Developer ID Application identity of the keychain, the app and its disk image are signed with it; when `RTYPE_NOTARY_KEY` (the path of an App Store Connect API key, `.p8`), `RTYPE_NOTARY_KEY_ID` and `RTYPE_NOTARY_ISSUER` are set too, the disk image is sent to Apple's notary service and the ticket is stapled to it, so Gatekeeper opens it without a warning, even offline. Without an identity the app is signed without one, and macOS asks the player, the first time, to allow it in System Settings > Privacy & Security. The app may load plugins signed by another developer (`com.apple.security.cs.disable-library-validation`, in `packaging/r-type.entitlements`). The server archive is not signed.
- The CI signs and notarizes the client when the repository holds the secrets `MACOS_CERTIFICATE_P12` (the Developer ID certificate and its key, as a base64 `.p12`), `MACOS_CERTIFICATE_PASSWORD`, `APPLE_API_KEY_P8` (the text of the `.p8`), `APPLE_API_KEY_ID` and `APPLE_API_ISSUER_ID`; the package check then requires Gatekeeper to accept the app. A pull request from a fork, which receives no secret, gets an app signed without an identity.
- macOS disk image: it opens on a window drawn in the colors of the game, with the app on the left, the Applications folder on the right and an arrow between them: dragging the app onto the folder installs it. That window is two files of `packaging/disk-image/`, the background (`background.tiff`, at its normal and Retina sizes) and the `.DS_Store` that sizes the window and places the icons (`DS_Store`); `make_layout.py` draws and writes both, on macOS, with Python and the `pillow`, `ds_store` and `mac_alias` packages (`pip install pillow ds_store mac_alias`, then `python3 packaging/disk-image/make_layout.py`). The `.DS_Store` finds the background through the name of the volume, `R-Type`: changing `CPACK_DMG_VOLUME_NAME` means running the script again with the new name.

The CI builds the packages of the three systems on every push to `main`, and on a pull request that changes `packaging/`, `assets/`, the root `CMakeLists.txt`, the presets, vcpkg or the workflows that build them, and keeps them 7 days as artifacts of the run (`packages-Linux`, `packages-Windows`, `packages-macOS`). Pushing a tag `v<version>` that matches the version of `project()` publishes them in a GitHub release, with notes listing the pull requests merged since the previous tag:

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

The server listens on UDP port 4243 of every IPv4 interface; without `-p`, on port 4242. It runs as a supervisor: it runs no game itself, and starts one `r-type_server --mode game` process per game, up to eight, sending each player to the game that has a place (see [the server page](https://nique-epi.github.io/R-Type/server/#the-three-roles-supervisor-game-and-master)). Each game writes its own journal, `r-type_game_<n>.log` next to the executable. The supervisor also serves its content pack, the entity sheets of its `assets/` and their sprite sheets, over HTTP on the TCP port of the same number (4243 here), so a player whose client has other sheets downloads them when it joins: a host behind a firewall or a router opens both the UDP and the TCP port (see [Tutorial: publish a mod](https://nique-epi.github.io/R-Type/tutorial-publish-a-mod/)).

```bash
./r-type_server --seed 42
```

The Bydos appear at heights drawn from the seed, the same in every game; without `--seed` each game draws one, and its journal records it (`wave spawning starts from the seed ...`) so the same game can be replayed. A seed makes the games not vanilla: their scores stay on the leaderboard of the server.

```bash
./r-type_server --set state-rate=30
```

`--set <name>=<value>` forces any setting of the server and wins over `server.cfg`, a file of commands the supervisor reads from the folder it is started from. Only the supervisor reads it: it runs its own lines (the port, the name, the database, the master) and writes the lines of its games (the seed, the state rate, the input delay, the plugins) to `r-type_games_<port>.cfg` next to the executable, which every game reads. A missing file is fine; the defaults apply. `server.cfg.example`, at the root of the repository, lists every setting with its default and its bounds: copy it to `server.cfg`. Every command and setting is described in [Configuration commands](https://nique-epi.github.io/R-Type/commands/).

A `plugin <name>` line of `server.cfg` loads the module of a shared library, `plugins/<name>.so` next to the executable (`.dll` on Windows), in every game process, so a feature is added without recompiling the server. A plugin that cannot be loaded stops the server with an error that names it, and `module <name> off` turns a loaded plugin off. The plugin must be built by the same compiler, from the same sources, as the server: see [Write a plugin](https://nique-epi.github.io/R-Type/modules/#write-a-plugin). The server has one, `game-statistics`: `plugin game-statistics` makes each game write a row per Bydo killed and the total of each game to `r-type_statistics_<port>.csv` next to the executable, with the port of that game (see [Game statistics](https://nique-epi.github.io/R-Type/server/#game-statistics-servergamestatisticsmodule)).

```
# server.cfg
set port 4300
module lobby off
```

```bash
./r-type_server --state-rate 30
```

The server sends each player 30 world states per second instead of 20, from 2 to 60: more states move the other ships more smoothly and use more bandwidth (about 350 bytes per state with four players and thirty entities, see [the server page](https://nique-epi.github.io/R-Type/server/)).

```bash
./r-type_server --input-delay 5
```

Every player's input is held for the same few ticks before it moves the ship (3 ticks, 50 ms, by default); `--input-delay` changes it, from 0 to 30 ticks; any other delay than 3 makes the games not vanilla.

```bash
./r-type_server --boss
```

Every game starts at its boss, without the waves, to test the fight: the Bydo Tyrant appears 4.4 seconds after the game starts, and the ships never die. The games are not vanilla; `set start-at-boss 1` in `server.cfg` does the same.

The server reads the waves of Bydos from `assets/waves/waves.toml`, in the `assets/` folder it finds next to its executable or in the folder above it. A missing or invalid file stops the game process with exit code 1, naming the file, the line and the key at fault in its journal, and the supervisor writes that the game crashed; the format is described in [Writing the waves file](https://nique-epi.github.io/R-Type/waves-file/). Both programs also read `assets/entities/`, one sheet per type of entity (the ship, the Bydos, the missiles), when they start, and stop the same way on an invalid sheet; the keys are described in [Writing an entity sheet](https://nique-epi.github.io/R-Type/entity-sheets/).

```bash
./r-type_client -h 192.168.1.20 -p 4243 -n ACE
```

Without `-h`, PLAY opens the server map: the servers the master lists, `http://127.0.0.1:8080` unless `set master-url` (in `client.cfg`, or `--set master-url=<url>`) names another, or the player chose another with MASTER (M) on the map, which `settings.ini` keeps, around the planet of the player by the ping the client measures, and the games of each; Enter or a click joins one. Start the master and a server registered with it first (see [the master page](https://nique-epi.github.io/R-Type/master/)): without the master, the map shows `MASTER UNREACHABLE`.

The client is given the IPv4 address and the port of the server, `127.0.0.1` and 4242 by default, and the nickname of the player, `PILOT` by default; the connection screen, which D (ADDRESS) opens from the map, starts with them. CONNECT on that screen joins any server, without the master: without a server, the screen shows `SERVER UNREACHABLE` after 5 seconds.

```bash
./r-type_client --game new
./r-type_client --game 2
```

`--game new` asks the server for a game of its own, which other players may join; `--game <n>` joins the game the server numbers `n` (`GAME 2` in the list of the master), and the connection screen says `REFUSED: NO SUCH GAME` when there is none. Without `--game` the client takes any game with a free seat. A game starts once all its connected players are ready, whoever opened it.

`--no-sound` starts the client without sound: the screens and the volumes work as usual, and nothing plays.

```bash
./r-type_client --network bad
```

The client's datagrams cross a simulated bad network: 300 ms more ping, 10 % of the datagrams lost, some doubled or out of order. Both programs take `--network` with `4g`, `wifi-busy` or `bad`, and `--lag`, `--jitter`, `--loss`, `--duplicates` and `--reorder` to set one value; the journal warns that the network is simulated (see [the network page](https://nique-epi.github.io/R-Type/network/)).

Without these options, `tests/tools/network-conditions/` applies the same scenarios to the loopback interface from the system: `sudo tests/tools/network-conditions/netem.sh apply --network bad` on Linux, `dummynet.sh` on macOS, then `clear` to remove them (see [the network page](https://nique-epi.github.io/R-Type/network/#network-conditions-from-the-system-teststoolsnetwork-conditions)).

```bash
./r-type_client --traffic-csv traffic.csv
```

The client and every game process write what crosses their socket to the journal every 10 s; with `--traffic-csv` the client also writes it to a file every second, by player and by message type, for a measurement campaign (see [the network page](https://nique-epi.github.io/R-Type/network/#traffic-counters-trafficcounters)). Given to the supervisor, `--traffic-csv traffic.csv` makes each game write its own `traffic_game_<n>.csv` in the same folder; the supervisor itself has no traffic file.

```bash
./r-type_server --mode master -p 8080
```

The same binary runs the master in this mode: the program that lists the game servers for the players, with the status of each, and will take the orders of an admin. `--mode` comes before every other option. Today it answers an HTTP API under `/api` on TCP port 8080 of every IPv4 interface (`curl http://127.0.0.1:8080/api/health` answers `{"version":"0.1.0"}`), keeps its data in `r-type_master.db` next to the executable, and reads `master.cfg`, modelled by `master.cfg.example` (see [the master page](https://nique-epi.github.io/R-Type/master/)). A host creates its game server with `POST /api/servers`, which answers the key of that server once; the master keeps only a hash of it. The host writes the key in the `server.cfg` of the game server, as `master-key` next to `master-url`, and the supervisor then registers once for all its games with `PUT /api/servers/me` and sends its heartbeat with `PUT /api/servers/me/heartbeat`, through `MasterClient`; a game process never talks to the master:

```bash
curl -X POST http://127.0.0.1:8080/api/servers -d '{"name":"Paris 1"}'
```

After each heartbeat, the master sends a probe over UDP to the port the game server registered, and lists as unreachable a server whose port it cannot reach: that port must be open to the master, as it is to the players.

Without `master-url` or `master-key`, a game server runs alone and is listed nowhere. A supervisor on the machine of the master sets `public-address` to the address players reach it at, or the master lists it at `127.0.0.1`.

- **Why HTTP:** the subject requires UDP for every message of a game and accepts TCP for other needs when it is justified. The master carries no message of a game, and the web administration the subject accepts needs HTTP in a browser, so one protocol serves the programs and the web site, readable with `curl`. A player still joins a server over UDP when the master is stopped.
- **Why a mode and not a third binary:** the subject lists two binaries, `r-type_server` and `r-type_client`. The master ships inside the first and reuses its stop on Ctrl-C, its configuration file and its journal; it still runs as a process of its own, so a master that stops stops no game.

Both choices are told in full in [Architecture](https://nique-epi.github.io/R-Type/architecture/#master).

The binary runs as a supervisor unless `--mode` says otherwise. A game that crashes stops only its own players, and Ctrl-C on the supervisor stops every game; see [the server page](https://nique-epi.github.io/R-Type/server/#supervisor-and-game-processes). `--mode game` is the process the supervisor starts for a game, with the options it gives it, never started by hand. There is no mode that runs games in the process of the supervisor.

`--help` prints the options of either program, `--mode master --help` those of the master, and `--mode game --help` those of a game process. An invalid argument stops it with a message and exit code 1.

### Web site

The web site lives in `web/`: a Next.js server, run with [bun](https://bun.sh) 1.3 and Node.js 22.12 or newer, that calls the API of the master. With a master running on port 8080:

```bash
cd web
bun install
bun run build
bun run start
```

The site is then on <http://localhost:3000>, in French and English (`/fr`, `/en`). The server reaches the master at `MASTER_URL` (`http://localhost:8080` by default); the browser only talks to the site. The download page reads the latest release of `GITHUB_REPOSITORY` (`nique-epi/R-Type` by default); `GITHUB_TOKEN` is optional and raises the limit of the GitHub API. `bun run dev` serves the same pages while you edit them. `bun run lint`, `bun run typecheck` and `bun run test` are the checks of the `web` CI workflow.

## Tests

```bash
cmake --workflow --preset test
```

```bash
cmake --workflow --preset sanitize
```

This builds everything in `build-sanitize/` with AddressSanitizer and UndefinedBehaviorSanitizer, then runs the tests; an error stops the test that caused it. It needs GCC or Clang, not MSVC, and LeakSanitizer reports leaks on Linux only. The Linux CI runs it, then starts `r-type_server --mode supervisor` with no player, so with no game running, stops it with SIGINT and requires exit code 0.

```bash
cmake --workflow --preset modules-off
```

This builds everything in `build-modules-off/` with every [build option](#build-options) off, then runs the tests: the tests of a module left out are not built, and a test that needs one is skipped. The Linux CI runs it, then starts and stops the supervisor in the same way.

```bash
./build/tests/malformed_datagrams --server 4242
```

Once the test preset is built, this sends the server running on port 4242 of the same machine malformed datagrams for one minute, then checks that it still answers a connection request. `--client <port>` does the same to a client; see [Network](https://nique-epi.github.io/R-Type/network/#malformed-datagrams-malformed_datagrams).

```bash
./build/tests/load_test_bots --bots 32 --seconds 60
```

This has 32 players with no window join the server on port 4242 of the same machine and play for one minute, then prints the world states each one received per second; the journal of each game gives the duration of its ticks every 10 s. Bots that connect at once are sent to the first game before it reports its players, so the ones it cannot seat are refused (`GameFull`) instead of filling the other games. See [Network](https://nique-epi.github.io/R-Type/network/#load-test-bots-load_test_bots).

```bash
sh tests/tools/bandwidth/campaign.sh ./r-type_server ./build/tests/load_test_bots
```

This runs the bandwidth campaign: a server and four bots from the same seed, three runs of one minute for each scenario, and one Markdown row per scenario with what one client received and sent per second. The results and the method are in [Network measurements](https://nique-epi.github.io/R-Type/measurements/).

## Logging

The client, the server and the master write a timestamped journal, `r-type_client.log`, `r-type_server.log` or `r-type_master.log` next to their executable (the client on macOS in `~/Library/Logs`, so it never writes into the signed app), and show the same lines in the terminal; each game of the server writes its own, `r-type_game_<n>.log`, at the level given to the supervisor; the first line gives the absolute path of the journal. Installed in a folder they cannot write to, they write no journal and say so in the terminal. The level and the output are chosen at launch:

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

The client reads `client.cfg` the same way, and `client.cfg.example` is its model (`set nickname`, `set server-address`, `set server-port`, `set master-url`, `module <name> on|off`, and `--set` to force a setting). It keeps the player's settings in `settings.ini`: in `$XDG_CONFIG_HOME/r-type` (or `~/.config/r-type`) on Linux, `~/Library/Application Support/R-Type` on macOS, `%LOCALAPPDATA%\R-Type` on Windows. Its path is logged at launch; deleting it brings back the settings of a first launch. The content packs of the servers it joined are kept next to it, in `content-packs/`, eight at most.

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

The documentation site is built with [Starlight](https://starlight.astro.build) and published at <https://nique-epi.github.io/R-Type/>. Each page is a Markdown file in `docs/src/content/docs/`. To add a module to the server or the client, start with the [Modules](https://nique-epi.github.io/R-Type/modules/) page and the [tutorial](https://nique-epi.github.io/R-Type/tutorial-write-a-module/). Three more tutorials [add a network message](https://nique-epi.github.io/R-Type/tutorial-add-a-network-message/), [run a server with several games](https://nique-epi.github.io/R-Type/tutorial-run-several-games/) and [publish a mod](https://nique-epi.github.io/R-Type/tutorial-publish-a-mod/).

To preview it locally, with Node.js 22.12 or newer:

```bash
cd docs
npm ci
npm run dev
```

`npm run build` fails on a broken internal link, like the `docs` check on pull requests.

## Contributing

How to branch, write a commit, write and test the code, open a pull request and know when a change is done is summed up in [the contributing guide](CONTRIBUTING.md).

## Documentation and protocol

- [Documentation site](https://nique-epi.github.io/R-Type/): architecture, engine, client, server, master, configuration and tutorials.
- [Network protocol](https://nique-epi.github.io/R-Type/protocol/): the specification of every datagram and exchange, written as an RFC (MUST, SHOULD and MAY as in RFC 2119), enough to write another client or server.
- [Contributing guide](CONTRIBUTING.md).

## Authors

Epitech tek3 team:

- Yannis Lorang ([@Yannislrg](https://github.com/Yannislrg))
- Loann Bideau ([@LoannBi](https://github.com/LoannBi))
- Nolan Fribault ([@nolanfrb](https://github.com/nolanfrb))
- Lukas ([@0rezh](https://github.com/0rezh))

## License

The code of this repository is released under the [MIT License](LICENSE). The assets keep the licences of their authors: see `assets/README.md` and the licence files in `assets/`.
