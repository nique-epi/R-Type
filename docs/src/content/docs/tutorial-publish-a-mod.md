---
title: "Tutorial: publish a mod"
description: A step-by-step tutorial that adds a resource, an entity sheet with its sprite sheet and its waves, to a server, runs the supervisor that serves them, opens its ports, and reads what the players and the journals show while their clients download the content of the server.
sidebar:
  label: "Tutorial: publish a mod"
---

This tutorial publishes a mod: a server that plays with an entity sheet the game does not ship, which every player gets without installing anything. It writes no code. The mechanism behind it is in [Content pack](/R-Type/protocol/#content-pack) and in [Server](/R-Type/server/#content-pack-servercontentmodule-contentpackdesk).

The steps were not run for this page: every line of a journal and of a screen is read from the code (`ServerContentModule.cpp`, `HandshakeAnswers.cpp`, `ConnectionScreenConstants.hpp`), with the variable parts in angle brackets. You need the project built (see the [README](https://github.com/nique-epi/R-Type#readme)).

## What a mod is here

A mod is a **resource**: a folder of `resources/`, beside the assets, that holds entity sheets, the sprite sheets their `[appearance]` names, and waves if it wants its enemies sent (see [Writing an entity sheet](/R-Type/entity-sheets/) and [Resources](/R-Type/server/#resources-ensure-lines)). An `ensure` line of `server.cfg` starts it; the assets are never touched. When a player joins, the server announces the fingerprint of its **content pack**, one file that holds the sheets and sprite sheets of the assets and of the resources. A client whose own `assets/` make the same pack plays at once; any other downloads the pack from the supervisor, over HTTP, on the TCP port of the same number as the UDP port, then draws the game with it. A behaviour, the code that steers a Bydo, never travels: a sheet can only name a behaviour the server already has.

| Step | You do | You see |
|---|---|---|
| 1 | Copy the build, its `assets/` and its `resources/` into a folder of the server | nothing yet |
| 2 | Write a resource: a manifest, a sheet, its sprite sheet and waves | nothing yet |
| 3 | Start the supervisor | its journal names the pack and the TCP port |
| 4 | Open the UDP and the TCP port | the players reach both |
| 5 | Join from a client of the plain game | `LOADING CONTENT`, then the waiting room |
| 6 | Join again | no download: the pack is in the cache |

## Step 1: a folder for the server

The supervisor reads the folder `assets/` next to its executable, or one level above it, whatever folder it is started from, and the folder `resources/` beside that `assets/`; its game processes run the same executable, so they read the same folders. Copy `r-type_server`, `assets/` and `resources/` into a folder of their own:

```bash
mkdir -p ~/rtype-mod
cp r-type_server ~/rtype-mod/
cp -R assets resources ~/rtype-mod/
cd ~/rtype-mod
```

## Step 2: write a resource

The repository ships one, `resources/crimson-drifter/`: a **Crimson Drifter**, a Drifter faster and sturdier, with sprites of its own. It is the model of this step:

```
resources/crimson-drifter/
├── resource.toml                                  what the resource is
├── entities/crimson_drifter.toml                  the sheet of the enemy
├── sprites/crimson-drifter/crimson_drifter.png    its two idle frames, 24 × 24 pixels each
└── waves/waves.toml                               the enemies of normal, the enemy among them
```

`resource.toml` holds a `description`, one line the host reads, and may hold `depends`, the resources to start before this one:

```toml
description = "A crimson Drifter, faster and sturdier, in the waves of normal"
```

The sheet is a copy of `assets/entities/drifter.toml` whose `top_speed`, `[gameplay.health]` and `[gameplay.score]` changed, and whose `[appearance]` names the sprite sheet of the resource, at the top-left corner of its image:

```toml
[appearance]
sheet = "sprites/crimson-drifter/crimson_drifter.png"
```

A sheet gives a Bydo; a wave makes it appear. `waves/waves.toml` of the resource is laid over the waves of the assets and holds only what it changes: the `enemies` of normal, with `crimson_drifter` among them. Every other key of normal, its boss and its look items included, and the other difficulties stay those of the assets (see [Writing the waves file](/R-Type/waves-file/#waves-of-a-resource)):

```toml
[difficulty.normal]
enemies = ["drifter", "weaver", "crimson_drifter", "voltjelly", "weaver", "reaper", "crimson_drifter"]
```

Then start the resource from `server.cfg`, in the folder the supervisor is started from:

```
ensure crimson-drifter
```

A resource adds content, it never replaces a sheet: a sheet or a sprite sheet that the assets or another resource already hold stops the supervisor. So keep the sprite sheets of a resource in a folder named after it (`sprites/crimson-drifter/`). Every path of a pack follows the same rules, checked when the supervisor starts:

- the name of a sheet, `crimson_drifter`, is lowercase letters, digits and `_`, 24 characters at most, and a server holds at most 32 sheets, its assets and resources together;
- the `sheet` path is 1 to 64 characters of lowercase letters, digits, `_`, `.`, `-` and `/`, with no empty folder name, `.` or `..`;
- the pack holds at most 64 files and 8 MiB.

## Step 3: start the supervisor

```bash
./r-type_server -p 4242
```

Its journal, `r-type_server.log` next to the executable, says that it listens on UDP, then that it serves the pack on the TCP port of the same number:

```text
[INFO ] [Server] - listening on UDP port 4242
[INFO ] [Server] - content pack <64 hexadecimal digits> (<size> bytes) served on TCP port 4242
```

The 64 digits are the fingerprint of the pack, and the players will download `<size>` bytes. The supervisor reads the assets and the resources once, when it starts: stop it before you change a sheet, a sprite sheet or a resource, then start it again.

When a line reads `: the content pack is not served` instead, the supervisor runs, but only the players who already hold the pack can play:

| The line starts with | Why | What to do |
|---|---|---|
| `the port is chosen by the system` | the UDP port is 0, so the TCP port has no number | give a port with `-p` or `set port` |
| `cannot listen for HTTP on 0.0.0.0 port 4242` | another program holds the TCP port | stop it, or start the supervisor on another port |

The games of a mod are not vanilla: a game process logs `not vanilla: the entity sheets are not the ones the game ships`, and its scores stay on the leaderboard of its own server (see [Vanilla games](/R-Type/server/#vanilla-games-vanillabreaches)).

## Step 4: open the ports

A player reaches the supervisor on two ports of the same number:

| Port | Protocol | Carries |
|---|---|---|
| 4242 | UDP | the game: the connection, the waiting room, the states |
| 4242 | TCP | the content pack, `GET /api/packs/<fingerprint>` |

Open both in the firewall of the machine, and forward both in the router when the players come from the Internet. With UDP alone, a player whose client lacks the pack is let in, then leaves with `PACK DOWNLOAD FAILED`. From another machine, the TCP port can be checked with the fingerprint of the journal:

```bash
curl -o pack.bin -w '%{http_code}\n' http://<address of the server>:4242/api/packs/<fingerprint>
```

It prints `200` and writes `<size>` bytes. Another fingerprint gets `404`, and more than three requests in a few seconds from one address get `429`: an address may download 3 times at once, then once every 10 s.

## Step 5: join from a client of the plain game

Start a client built from the plain repository, whose `assets/` have no `crimson_drifter.toml`, and connect to the server (see [Client](/R-Type/client/#connection-screen-connectionscreen-iserverconnection)). The connection screen shows:

1. `CONNECTING...`;
2. `LOADING CONTENT <n>%`, from 0 to 100, while the pack downloads; the player already holds a seat, and the others see it in their waiting room, not ready;
3. `CONNECTED AS PLAYER <n>` for a second, then the waiting room.

The journal of the client says, under `Connection`:

```text
[INFO ] [Connection] - the server plays with other content: pack <fingerprint>, <size> bytes
[INFO ] [Connection] - accepted as player <n>
[INFO ] [Connection] - the content pack of the server is loaded
```

In the game, the Crimson Drifters of the wave are drawn from the sprite sheet of the pack. A client whose `assets/` hold the same sheets and sprite sheets as the server, byte for byte, makes the same fingerprint and logs none of the content lines: it plays with its own files at once.

## Step 6: join again

The client keeps the pack in `content-packs/` of its settings folder, the folder of `settings.ini` (see the [README](https://github.com/nique-epi/R-Type#readme)), as `<fingerprint>.pack`, with its sprite sheets in a folder of the same name. The next time it joins this server, it reads the pack from there, checks it again, and goes from `CONNECTING...` to `CONNECTED AS PLAYER <n>` with no `LOADING CONTENT`. It keeps the 8 packs it used last.

Any change to a sheet or a sprite sheet of the server, a value of `[gameplay]` included, makes another pack with another fingerprint, so every client downloads the new pack once.

## When something goes wrong

| The connection screen shows | What it means |
|---|---|
| `PACK DOWNLOAD FAILED` | the client could not download the pack: nothing answers on the TCP port (closed, not forwarded, or the supervisor logged `the content pack is not served`), the download was cut three times, or the supervisor answered with another status or more bytes than announced, such as a `429` after too many tries from one address. The client left the server, which freed the seat |
| `PACK REFUSED` | the pack came, but its size or its fingerprint is not the one the game announced, or it holds a sheet this client cannot read, such as a key of `[appearance]` it does not know, or a `sheet` that names a sprite sheet the pack does not hold. The assets of the server changed while it ran (restart the supervisor), or the client is older than the server |
| `REFUSED: V<client>, SERVER V<server>` | the client and the server speak two versions of the protocol: build both from the same version |
| `SERVER UNREACHABLE` | the UDP port does not answer: this is not the pack |

| The supervisor stops at start with | What it means |
|---|---|
| `Refused content pack: "<path>" may not name a file` | a `sheet` path holds a character a pack refuses, such as an uppercase letter or a space |
| `Refused content pack: <n> files, more than a pack may hold` | more than 64 files: fewer sheets, or sheets that share a sprite sheet |
| `Refused content pack: <n> bytes, more than a pack may hold` | the sprite sheets are too large for 8 MiB |
| an error naming a line of `crimson_drifter.toml` | the sheet is invalid; the message names the line and the key (see [Writing an entity sheet](/R-Type/entity-sheets/#when-a-program-refuses-a-sheet)) |
| `The resource "<name>" has no manifest in <folder>` | the `ensure` line names a folder of `resources/` that is missing or holds no `resource.toml` |
| `The resources depend on each other in a circle: <names>` | the `depends` of the manifests make a circle |
| `The content file <path> is in more than one folder: <folders>` | two folders hold the sprite sheet a sheet names: give the resource its own folder of sprites |
| `Two entity sheets are named <name>` | the resource holds a sheet the assets or another resource already hold |
