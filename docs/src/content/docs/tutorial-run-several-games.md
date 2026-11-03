---
title: "Tutorial: run a server with several games"
description: A step-by-step tutorial that runs one server hosting several games, fills them, reads what happened in the journal, and runs several servers side by side.
---

This tutorial runs one `r-type_server` that hosts several games at once, fills a first game until the next player lands in a second one, and reads what happened in the journal. It writes no code. The reasoning behind what you will see is in [Several games at once](/R-Type/server/#several-games-at-once).

Every server command below was run before this page was written, and the lines come from the journals of those runs; the players of those runs were scripted over UDP, speaking the [protocol](/R-Type/protocol/), rather than client windows. You need the project built (see the [README](https://github.com/nique-epi/R-Type#readme)).

The server has **no dashboard yet**: the journal is the only way to see its games, and it does not number them, though it gives how many are in progress every 10 s (see [Metrics](/R-Type/server/#metrics-servermetricsmodule)). A dashboard needs the master server, which does not exist yet; this page will gain its steps once it does.

## What you will have at the end

| Step | You do | You see |
|---|---|---|
| 1 | Start one server | Eight games ready |
| 2 | Connect five players | The fifth lands in a second game |
| 3 | Start a game, then connect one more player | The new player waits in another game |
| 4 | Change the number of games | The server refuses the player above the new limit |
| 5 | Start two servers | Two journals, on two ports |

## Step 1: start the server

From the repository root, where the build writes `r-type_server`:

```bash
./r-type_server -p 4242 --seed 42
```

There is nothing to ask for: a server always hosts `DEFAULT_HOSTED_GAME_COUNT` (8) games, created when it starts and kept until it stops. `--seed` is optional and makes the Bydos come out the same way at each launch. Each line goes both to the terminal and to the journal `r-type_server.log`, next to the executable (each line starts with a time, left out here):

```text
[INFO ] [Server] - server starting
[INFO ] [Server] - listening on UDP port 4242
[INFO ] [Server] - waves file assets/waves/waves.toml: 3 difficulties, games play normal
[INFO ] [BydoSpawner] - Bydo spawning starts from the seed 42
[INFO ] [BydoSpawner] - Bydo spawning starts from the seed 65578
```

The `BydoSpawner` line appears eight times, once per game. The seed of each game is the seed given plus `GAME_SEED_STRIDE` (65536) times its rank from 0, so 42, then 65578, then 131114. It is the one line that shows how many games the server holds.

## Step 2: fill the first game

Start five clients, in five terminals, each with its own nickname, and press CONNECT in each:

```bash
./r-type_client -n ONE
```

A nickname is unique on the server, whichever game holds it: a second `ONE` is refused with `NicknameTaken`. The journal shows who joined:

```text
[INFO ] [Lobby] - player 1 joined from 127.0.0.1 port 56370
[INFO ] [Lobby] - player 2 joined from 127.0.0.1 port 56052
[INFO ] [Lobby] - player 3 joined from 127.0.0.1 port 54222
[INFO ] [Lobby] - player 4 joined from 127.0.0.1 port 65219
[INFO ] [Lobby] - player 1 joined from 127.0.0.1 port 58434
```

A game holds `MAXIMUM_PLAYERS` (4) players, so the fifth starts a second one: its number is 1 again. That number is its place in its own game, and the journal names no game, so a second "player 1" is how you read it. The same number also picks the color of the ship, so the first and the fifth player have the same color without ever meeting.

Now mark every client ready. The games start one by one, each with its own players:

```text
[INFO ] [Instances] - game started, players connected: 4
[INFO ] [Instances] - game started, players connected: 1
```

Each game waits only for its own players, and starts as soon as all of them are ready, however few. Close the clients: each game finishes on its own once its last player leaves, and waits for players again with a new world.

## Step 3: join while a game runs

A new player never joins a game in progress, even one with free seats. Connect one client and mark it ready, so its game starts with one player, then connect a second client:

```text
[INFO ] [Lobby] - player 1 joined from 127.0.0.1 port 53106
[INFO ] [Instances] - game started, players connected: 1
[INFO ] [Lobby] - player 1 joined from 127.0.0.1 port 57591
```

The second client waits alone in the next game, while the first one plays. `Games::joinable()` picks the first game waiting for players with a free seat, so two friends who connect while nothing else happens land together, and two who connect around the start of a game may not. There is no way yet to choose a game.

A server refuses a player only when no game waits with a free seat. Of 33 players connected to a server of eight games, 32 are accepted, and the last one is refused with `GameFull`. When a game in progress comes first, the refusal is `GameInProgress` instead: the first game answers the refusal, in the state it is in. The client tells the player on its connection screen.

## Step 4: change the number of games

No launch option changes the number of games, and neither does a file: it is a constant, so changing it takes a build. In `src/server/ServerConstants.hpp`:

```cpp
constexpr std::size_t DEFAULT_HOSTED_GAME_COUNT = 2;
```

```bash
cmake --workflow --preset build
```

The server now creates two games, so the `BydoSpawner` line appears twice, and the ninth player is refused. Put the value back before you commit. A test that needs another number sets `GamesSettings::gameCount` itself, so no test depends on this constant.

More games cost more per tick, and the journal gives that cost every 10 s (see [Metrics](/R-Type/server/#metrics-servermetricsmodule)). Every game runs on the simulation thread, one after the other, so a slow game slows the others, and an exception in one stops the server. The tick loop writes a line at the `warn` level when ticks stall; if you see `stalls since the previous report` in the journal, the server is holding more than the machine can run.

## Step 5: run two servers side by side

Several servers can run on one machine, each on its own port, with nothing shared between them. Give each its own journal, or their lines mix in one file:

```bash
./r-type_server -p 4251 --log-file first.log
```

```bash
./r-type_server -p 4252 --log-file second.log
```

Each client names the server it joins:

```bash
./r-type_client -p 4252 -n TWO
```

```text
[INFO ] [Server] - listening on UDP port 4252
[INFO ] [Lobby] - player 1 joined from 127.0.0.1 port 54489
```

Two servers know nothing of each other: a nickname is unique per server, and a full server sends nobody to the other one. Stop each with Ctrl-C: it tells its players it shuts down, writes `server stopped`, and exits with code 0.

## If it does not work

| You see | Cause | Fix |
|---|---|---|
| `Cannot open a UDP socket on 0.0.0.0 port 4242: Address already in use`, exit code 1 | Another server holds the port | Give another port with `-p`, or stop the other server |
| The server stops at launch, naming `waves.toml` or `No assets folder found` | `assets/waves/waves.toml` was moved, or the executable was copied away from `assets/` | Put them back: see [Writing the waves file](/R-Type/waves-file/) |
| `SERVER UNREACHABLE` on the client after 5 seconds | The server is not running, or the client names another port | Check `listening on UDP port` in the journal, and `-p` on the client |
| `REFUSED: CALLSIGN TAKEN` on the client | Another player of that server holds it, or left less than 20 s ago without saying so | Pick another nickname, or wait for the seat to be freed |
| The lines of two servers in one journal | Both write to the default `r-type_server.log` next to the executable, whatever folder they were started in | Give each its own `--log-file` |
