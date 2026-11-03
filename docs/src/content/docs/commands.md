---
title: Configuration commands
description: The language of server.cfg and client.cfg, every command and setting the server and the client accept, and what happens to a line that is refused.
sidebar:
  label: Configuration commands
---

`server.cfg` and `client.cfg` are files of commands: one command per line, read when the program starts, before its modules start. This page lists every command and every setting they accept. How the interpreter is built, and how a module adds a command or a setting, is on the [Engine](/R-Type/engine/#commands-and-configuration-commandinterpreter) page.

The repository holds a model of each file, `server.cfg.example` and `client.cfg.example`, at its root. Each one lists every setting with its default, so it changes nothing as it is. Copy it to `server.cfg` or `client.cfg` in the folder the program is started from: the program never reads the `.example` file.

## The language

A line is a command name followed by its arguments, separated by spaces:

```
set port 4300
```

- A pair of double quotes keeps the spaces between them in one word: `set nickname "Space Cowboy"`. Inside quotes, `\"` stands for a quote and `\\` for a backslash. A backslash before any other character is a plain backslash. A quote left open ends with the line.
- `#` outside quotes starts a comment that runs to the end of the line. A blank line, or a line holding only a comment, does nothing.
- Names and words are compared exactly, upper and lower case included: `set Port 4300` is refused, `module lobby OFF` too.
- There are no variables, no conditions, no loops and no command that reads another file. A line is run once, in the order of the file.

## The two commands

| Command | Program | Effect |
|---|---|---|
| `set <setting> <value>` | server and client | Gives a setting a value, after checking it. |
| `module <name> on\|off` | server and client | Turns a module of the program on or off before the modules start. |

No other command exists today. The interpreter keeps a help text for each command, but nothing prints it yet. A module that registers a command in its `install()` makes it usable in the file with no other change.

### `set`

`set` takes exactly two words: the name of a setting and its value. A whole number is digits only, with no sign and no space. The value is written only once it has been checked, so a refused line leaves the previous value.

Server settings:

| Setting | Value | Default | Effect |
|---|---|---|---|
| `port` | 1 to 65535 | 4242 | The UDP port the server listens on. |
| `seed` | 0 to 4294967295, or `none` | `none` | The seed of the first game; `none` draws one at random. |
| `state-rate` | 2 to 60 | 20 | The world states sent to each player per second. |
| `input-delay` | 0 to 30 | 3 | The ticks every player's input is held before it is applied. |

Client settings:

| Setting | Value | Default | Effect |
|---|---|---|---|
| `nickname` | 1 to 16 bytes of UTF-8, with no control character | `PILOT` | The nickname the connection screen starts with. |
| `server-address` | an IPv4 address written as four numbers | `127.0.0.1` | The address the connection screen starts with. A host name is refused. |
| `server-port` | 1 to 65535 | 4242 | The port the connection screen starts with. |

The client settings only change what the connection screen offers first: the player can still edit the fields.

### `module`

`module` takes exactly two words: the name of a module and `on` or `off`. A module that is off is installed and never started, and the work it added to the loop in `install()` is taken out, so it opens nothing and runs nothing. Every module is on until a line turns it off.

A module cannot be turned off while a module that stays on needs it, and cannot be turned on while a module it needs is off. The check uses the state at the time of the line, so turn off the dependents first and turn on the dependencies first.

Server modules:

| Module | Needs | Does |
|---|---|---|
| `logger` | nothing | Applies the logging options when it starts. |
| `network` | `logger` | Owns the UDP socket and the network thread. |
| `games` | `network` | Hosts the games. |
| `lobby` | `network`, `games` | Answers the connection requests of the players. |
| `metrics` | `games` | Measures the ticks and writes a report to the journal. |

Client modules:

| Module | Needs |
|---|---|
| `logger` | nothing |
| `screen-registry` | nothing |
| `assets` | `logger` |
| `connection` | `logger` |
| `settings` | `logger` |
| `window` | `settings` |
| `audio` | `assets`, `settings` |
| `menus`, `lobby`, `game` | `screen-registry` |
| `screens` | `assets`, `settings`, `audio`, `window`, `connection`, `screen-registry`, `menus`, `lobby`, `game` |

Turning off a module the program cannot run without is accepted when nothing that stays on needs it, and the program then does less. For example, `module lobby off` on the server leaves a server that accepts nobody. On the client, almost every module is needed by `screens`, so turning one off is refused unless `screens` goes off first.

## What happens to a bad line

A refused line never stops the program. The reading goes on with the next line, and the program logs the refusal at the `warn` level once its journal is open, with the file and the line number. A line that comes from a launch option names the launch option instead of a file.

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

## Where the settings come from

From the lowest priority to the highest:

1. The default of the setting, written in the module that declares it.
2. The file: `server.cfg` or `client.cfg`, in the folder the program is started from.
3. The launch options. `-p`, `--seed`, `--state-rate` and `--input-delay` on the server, `-h`, `-p` and `-n` on the client, and `--set <name>=<value>` on both, are turned into `set` lines that run after the file. The last value given wins, so a launch option always wins over the file.

A missing file is not an error: the defaults apply and the journal says `server.cfg not found, the defaults apply` (or `client.cfg`). The `info` lines are hidden at the default log level; start the program with `--log-level info` to see them.

A module reads its settings when it starts, never when it installs, because the file is read between the two. A change to the file therefore takes effect at the next launch.

```bash
./r-type_server --set state-rate=30 --seed 7
```

The same two lines in `server.cfg` would be `set state-rate 30` and `set seed 7`.

## Not in the files

The client keeps the settings a player changes in its options menu in `settings.ini`, not in `client.cfg`. The two files do not read each other: `client.cfg` is written by hand.
