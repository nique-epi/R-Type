---
title: Configuration commands
description: The language of server.cfg, client.cfg and master.cfg, every command and setting the server, the client and the master accept, and what happens to a line that is refused.
sidebar:
  label: Configuration commands
---

`server.cfg`, `client.cfg` and `master.cfg` are files of commands: one command per line, read when the program starts, before its modules start. This page lists every command and every setting they accept. How the interpreter is built, and how a module adds a command or a setting, is on the [Engine](/R-Type/engine/#commands-and-configuration-commandinterpreter) page.

The repository holds a model of each file, `server.cfg.example`, `client.cfg.example` and `master.cfg.example`, at its root. Each one lists every setting with its default, so it changes nothing as it is. Copy it to `server.cfg`, `client.cfg` or `master.cfg` in the folder the program is started from: the program never reads the `.example` file.

## The language

A line is a command name followed by its arguments, separated by spaces:

```
set port 4300
```

- A pair of double quotes keeps the spaces between them in one word: `set nickname "Space Cowboy"`. Inside quotes, `\"` stands for a quote and `\\` for a backslash. A backslash before any other character is a plain backslash. A quote left open ends with the line.
- `#` outside quotes starts a comment that runs to the end of the line. A blank line, or a line holding only a comment, does nothing.
- Names and words are compared exactly, upper and lower case included: `set Port 4300` is refused, `module lobby OFF` too.
- There are no variables, no conditions, no loops and no command that reads another file. A line is run once, in the order of the file.

## The commands

| Command | Program | Effect |
|---|---|---|
| `set <setting> <value>` | server, client and master | Gives a setting a value, after checking it. |
| `module <name> on\|off` | server, client and master | Turns a module of the program on or off before the modules start. |
| `plugin <name>` | server, client and master | Loads a plugin, a module built as a shared library, before the modules install. |
| `plugin-folder <folder>` | server, client and master | Sets the folder the `plugin` lines read. |
| `ensure <resource>` | server | Starts a resource: its sheets, sprite sheets and waves are laid over the assets. |

No other command exists. The interpreter keeps a help text for each command, but nothing prints it yet. A module that registers a command in its `install()` makes it usable in the file with no other change.

### `set`

`set` takes exactly two words: the name of a setting and its value. A whole number is digits only, with no sign and no space. The value is written only once it has been checked, so a refused line leaves the previous value.

Server settings. Only the supervisor reads `server.cfg`: it runs the lines of its own settings, and hands the lines of the settings of the games to every game it starts, through `r-type_games_<port>.cfg` (see [Server](/R-Type/server/#configuration-servercfg)). The column Read by says which:

| Setting | Read by | Value | Default | Effect |
|---|---|---|---|---|
| `port` | the supervisor | 1 to 65535 | 4242 | The UDP port the server listens on. |
| `seed` | the games | 0 to 4294967295, or `none` | `none` | The seed of each game; `none` draws one at random. |
| `state-rate` | the games | 2 to 60 | 20 | The world states sent to each player per second. |
| `state-encoding` | the games | `delta` or `full` | `delta` | How each world state is written: as the difference from the newest one the player acknowledged, or with every entity. `delta` also writes a state with every entity when the player acknowledged none, or when the newest it acknowledged is more than 60 ticks older (`MAXIMUM_REFERENCE_DISTANCE`). |
| `input-delay` | the games | 0 to 30 | 3 | The ticks every player's input is held before it is applied. |
| `database` | the supervisor | a path | empty | The SQLite file the leaderboard of the server is kept in, given to every game; empty for `r-type_server_<port>.db` next to the executable, with the port of the supervisor. |
| `master-url` | the supervisor | `http://<host>[:<port>]` | empty | The master the server is listed on; empty to run alone. |
| `master-key` | the supervisor | a key | empty | The key the master gave when the host created the server; empty to run alone. |
| `server-name` | the supervisor | 1 to 32 bytes | `R-Type server` | The name players read in the list of servers and in the answer to an information request. |
| `game-statistics-file` | the games | a path | empty | Declared by the `game-statistics` plugin, only once a `plugin game-statistics` line loads it: the CSV file the statistics of each game are named after, each game putting `_<port>` before its extension, with its own port; empty for `r-type_statistics_<port>.csv` next to the executable. |

Client settings:

| Setting | Value | Default | Effect |
|---|---|---|---|
| `nickname` | 1 to 16 bytes of UTF-8, with no control character | `PILOT` | The nickname the connection screen starts with. |
| `server-address` | an IPv4 address written as four numbers | `127.0.0.1` | The address the connection screen starts with. A host name is refused. |
| `server-port` | 1 to 65535 | 4242 | The port the connection screen starts with. |

The client settings only change what the connection screen offers first: the player can still edit the fields.

Master settings (see [Master](/R-Type/master/#configuration-mastercfg)):

| Setting | Value | Default | Effect |
|---|---|---|---|
| `port` | 1 to 65535 | 8080 | The TCP port the HTTP API listens on, on every interface. |
| `database` | a path | empty | The SQLite file of the master; empty for `r-type_master.db` next to the executable. |

### `module`

`module` takes exactly two words: the name of a module and `on` or `off`. A module that is off is never installed: the `module` lines run once before any install, so it provides no service, adds no work, opens nothing and runs nothing. The file is read three times: first for the `plugin` and `plugin-folder` lines alone, then for the `module` lines alone, both before any install, and a last time with every command once every module is installed, so a `set` line finds the settings the modules declared. The `module` lines come after the `plugin` ones, so a plugin can be turned off like any module. Every module is on until a line turns it off.

In `server.cfg`, a `module` line on a module of the supervisor runs in the supervisor, and a line on any other name goes to the games, which run it.

A module cannot be turned off while a module that stays on needs it, and cannot be turned on while a module it needs is off. The check uses the state at the time of the line, so turn off the dependents first and turn on the dependencies first.

### `plugin`

`plugin` takes exactly one word: the name of a plugin, made of letters, digits, `-` and `_`. It opens `<folder>/<name>.so` on Linux and macOS, `<folder>/<name>.dll` on Windows, and adds the module the library creates to the end of the list of modules, in the order of the lines. The folder is `plugins/` next to the executable until `plugin-folder` changes it. The same line run again does nothing: the file is read three times, and a plugin is loaded once.

A plugin that cannot be loaded does not refuse the line: it stops the program before any module installs, and the program prints `game stopped: <message>` (`client stopped`, `master stopped`) and exits with code 1. On the server only a game process loads plugins, so a plugin that cannot be loaded stops each game as it starts, and the supervisor writes in its journal that the game crashed. The message names the plugin: a file that is not in the folder, a library that exports no `rtypePluginEntry`, a plugin built for another plugin API version (the message gives both numbers), a name with another character than the allowed ones, or a name a module already has. A plugin must be built by the same compiler, from the same sources, as the program (see [Write a plugin](/R-Type/modules/#write-a-plugin)).

### `plugin-folder`

`plugin-folder` takes exactly one word: a folder, read from the folder the program is started from when it is relative, like the file itself. It sets where the `plugin` lines look, and it comes before every `plugin` line: after one, a line that names another folder is refused, and a line that names the same folder again does nothing.

### `ensure`

`ensure` takes exactly one word: the name of a resource, 1 to 32 lowercase letters, digits, `-` and `_`. It starts the folder of that name in `resources/`, beside the assets folder, with the resources its manifest depends on before it; a resource named twice, or brought by another, starts once (see [Resources](/R-Type/server/#resources-ensure-lines)). It is a line of the games: the supervisor hands it to them, like `plugin`, and also reads it, to serve the content of the resources. Both read the `ensure` lines before they build their modules, so a line that names no resource or two, a name with another character, a missing resource, a manifest the reader refuses or dependencies in a circle stop the supervisor at start, with `supervisor stopped: <message>`. The client and the master have no such command.

Modules of the supervisor:

| Module | Needs | Does |
|---|---|---|
| `logger` | nothing | Applies the logging options when it starts. |
| `network` | `logger` | Owns the UDP socket and the network thread. |
| `content` | `network` | Serves the content pack over HTTP, on the TCP port of the number of `port`. |
| `supervised games` | `network` | Starts, follows and stops the game processes. |
| `supervisor lobby` | `network`, `supervised games` | Sends each player to a game, or starts one. |
| `information` | `network`, `supervised games` | Answers the information requests. |
| `master` | `network`, `supervised games`, `information` | Keeps the master told of the games. |

Modules of a game process, which every `module` line on another name reaches:

| Module | Needs | Does |
|---|---|---|
| `logger` | nothing | Applies the logging options when it starts; provides the journal a plugin writes to. |
| `network` | `logger` | Owns the UDP socket and the network thread. |
| `storage` | `logger`, `network` | Opens the database of the leaderboard. |
| `games` | `network`, `storage` | Hosts the game. |
| `lobby` | `network`, `games` | Answers the connection requests of the players. |
| `metrics` | `games`, `network` | Measures the ticks and writes a report to the journal. |
| `supervisor link` | `network`, `games` | Reports the game to the supervisor; uses `metrics` when it is on. |
| `game-statistics` | `logger`, `network`, `games` | A plugin, loaded by `plugin game-statistics`: writes a row per Bydo killed and the total of each game to a CSV file (see [Server](/R-Type/server/#game-statistics-servergamestatisticsmodule)). |

Client modules:

| Module | Needs |
|---|---|
| `logger` | nothing |
| `assets` | `logger` |
| `connection` | `logger` |
| `settings` | `logger` |
| `window` | `settings` |
| `audio` | `assets`, `settings` |
| `screens` | `assets`, `settings`, `window`, `connection`; it uses `audio` when it is on, and plays nothing otherwise |
| `menus`, `lobby`, `game` | `screens` |

Master modules:

| Module | Needs | Does |
|---|---|---|
| `logger` | nothing | Applies the logging options when it starts. |
| `storage` | `logger` | Opens the database of the master. |
| `probe` | `logger` | Opens the UDP socket the probes of the game servers leave from. |
| `http` | `logger` | Opens the port of the HTTP API. |
| `servers` | `logger`, `storage`, `probe`, `http` | Creates the game servers with their keys, takes their registration and their heartbeats, and probes their UDP port. |

Turning off a module the program cannot run without is accepted when nothing that stays on needs it, and the program then does less. For example, `module lobby off` on the server leaves a server that accepts nobody. On the client, `menus`, `lobby` and `game` need `screens`, and `screens` needs five modules, so turning one of those off is refused until the modules that need it are off first.

## What happens to a bad line

A refused line never stops the program, with two exceptions: a `plugin` line that names a plugin that cannot be loaded (see [`plugin`](#plugin)), and a wrong `ensure` line (see [`ensure`](#ensure)). The reading goes on with the next line, and the program logs the refusal at the `warn` level once its journal is open, with the file and the line number. A line that comes from a launch option names the launch option instead of a file.

| Cause | Message |
|---|---|
| Unknown command | `unknown command '<name>' in line '<line>'` |
| Any refusal by a command | `command '<name>' refused line '<line>': <reason>` |

The reasons:

| Command | Reason |
|---|---|
| `set` | `expected: set <setting> <value>` when the number of words is not two |
| `set` | `unknown setting '<name>'; known settings: <list>` |
| `set` | `value '<text>' is not a whole number` |
| `set` | `value <number> is outside <lowest>..<highest>` |
| `set` | the reason the setting gives, for a nickname or an address that is not valid |
| `module` | `expected: module <name> on\|off` when the number of words is not two |
| `module` | `no module named '<name>'; modules: <list>` |
| `module` | `expected 'on' or 'off', got '<word>'` |
| `module` | `module '<a>' needs module '<b>', which is off` (turning on) or `which would be off` (turning off) |
| `plugin` | `expected: plugin <name>` when the number of words is not one |
| `plugin` | `plugins load before the modules install` for a plugin not loaded yet, once the modules are listed |
| `plugin-folder` | `expected: plugin-folder <folder>` when the number of words is not one |
| `plugin-folder` | `plugin-folder comes before every plugin line` for another folder, after a `plugin` line |

An `ensure` line is never refused: a wrong one stops the server before its modules are built (see [`ensure`](#ensure)).

## Where the settings come from

From the lowest priority to the highest:

1. The default of the setting, written in the module that declares it.
2. The file: `server.cfg`, `client.cfg` or `master.cfg`, in the folder the program is started from.
3. The launch options. `-p`, `--seed`, `--state-rate`, `--input-delay` and `--database` on the supervisor, which hands the lines of the settings of the games to its games after the lines of the file, `-h`, `-p` and `-n` on the client, `-p` and `--database` on the master, and `--set <name>=<value>` on all three, are turned into `set` lines that run after the file. The last value given wins, so a launch option always wins over the file.

A missing file is not an error: the defaults apply and the journal says `server.cfg not found, the defaults apply` (or `client.cfg`, `master.cfg`). The `info` lines are hidden at the default log level; start the program with `--log-level info` to see them.

A module reads its settings when it starts, never when it installs, because the file is read between the two. A change to the file therefore takes effect at the next launch.

```bash
./r-type_server --set state-rate=30 --seed 7
```

The same two lines in `server.cfg` would be `set state-rate 30` and `set seed 7`.

## Not in the files

The client keeps the settings a player changes in its options menu in `settings.ini`, not in `client.cfg`. The two files do not read each other: `client.cfg` is written by hand.
