---
title: "Tutorial: run a server with several games"
description: A step-by-step tutorial that runs one supervisor, fills a first game until the next player gets a second game process, reads what happened in the journals, and runs several servers side by side.
---

This tutorial runs one `r-type_server`, the supervisor, fills a first game until the next player gets a game of its own, and reads what happened in the journals. It writes no code. The reasoning behind what you will see is in [Supervisor and game processes](/R-Type/server/#supervisor-and-game-processes).

The commands of steps 1 and 2 were run on 6 November 2026 before this page was written, and their lines come from the journals of that run; the players were `load_test_bots` (see [Network](/R-Type/network/#load-test-bots-load_test_bots)) rather than client windows. The other steps were not run for this page: what they say is read from the code. You need the project built (see the [README](https://github.com/nique-epi/R-Type#readme)).

The server has **no dashboard yet**: the journals are the only way to see its games, one for the supervisor and one per game. A supervisor that has a `master-url` and a `master-key` tells the master its games, and `GET /api/servers/{id}/games` lists them (see [Master](/R-Type/master/#listing-the-games-of-a-server)).

## What you will have at the end

| Step | You do | You see |
|---|---|---|
| 1 | Start the supervisor | It listens, and runs no game yet |
| 2 | Connect four players, then a fifth | A first game process for the four, a second one for the fifth |
| 3 | Start a game, then connect one more player | The new player gets another game |
| 4 | Fill every place | The supervisor refuses the player above the limit |
| 5 | Start two supervisors | Two journals, on two ports |

## Step 1: start the supervisor

From the repository root, where the build writes `r-type_server`:

```bash
./r-type_server -p 4242 --seed 42
```

`--mode supervisor` is the default, so it needs no option. `--seed` is optional and makes the Bydos come out the same way in every game. Each line goes both to the terminal and to the journal of the supervisor, `r-type_server.log` next to the executable (each line starts with a time, left out here):

```text
[INFO ] [Server] - server starting
[INFO ] [Server] - listening on UDP port 4242
[INFO ] [Supervisor] - configuration of the games written to /path/to/r-type_games_4242.cfg: 1 lines
[INFO ] [Server] - no master-url or master-key: the server runs alone
[INFO ] [Server] - server.cfg not found, the defaults apply
```

The supervisor runs no game yet: it starts one when a player needs it, up to `MAXIMUM_RUNNING_GAMES` (8). The one line of `r-type_games_4242.cfg` is `set seed 42`: the seed is a setting of the games, so the supervisor hands it to every game it starts (see [Configuration](/R-Type/server/#configuration-servercfg)).

## Step 2: fill the first game

Start four clients, in four terminals, each with its own nickname, and press CONNECT in each:

```bash
./r-type_client -n ONE
```

The supervisor starts a game process for the first one and sends the others to it:

```text
[INFO ] [Supervisor] - game 0 started on port 60884
[INFO ] [Supervisor] - game 0 is ready
```

Game 0 writes its own journal, `r-type_game_0.log` next to the executable, which holds what the journal of a server held, for that game only. Its three commands are the line of the file and the port and the database its command line forces:

```text
[INFO ] [Server] - listening on UDP port 60884
[INFO ] [WaveSpawner] - wave spawning starts from the seed 42
[INFO ] [Server] - configuration read from /path/to/r-type_games_4242.cfg: 3 commands run
[INFO ] [Lobby] - player 1 joined from 127.0.0.1 port 60823
[INFO ] [Lobby] - player 2 joined from 127.0.0.1 port 57161
[INFO ] [Lobby] - player 3 joined from 127.0.0.1 port 56508
[INFO ] [Lobby] - player 4 joined from 127.0.0.1 port 59064
```

A game holds `MAXIMUM_PLAYERS` (4) players, so connect a fifth client a few seconds later: the supervisor starts a second process for it, which writes `r-type_game_1.log`, where the fifth player is player 1 again:

```text
[INFO ] [Supervisor] - game 1 started on port 52082
[INFO ] [Supervisor] - game 1 is ready
```

The number of the journal, 0 then 1, grows with each game the supervisor starts. The master and the players read another number, the place of the game, from 1 to 8: game 0 holds place 1 and is `GAME 1`, game 1 holds place 2. A nickname is unique in a game, not on the supervisor.

Now mark every client ready. Each game starts as soon as all of its players are ready, however few, and writes it in its own journal:

```text
[INFO ] [Instances] - game started, players connected: 4
```

Close the clients: each game finishes once its last player leaves, then its process stops by itself 10 s later (`EMPTY_GAME_GRACE`), and the supervisor writes `game 1 ended` and frees its place.

## Step 3: join while a game runs

A new player never joins a game in progress, even one with free seats. Connect one client and mark it ready, so its game starts with one player, then connect a second client: the supervisor finds no game waiting for players, so it starts another process for it. Two friends who connect while nothing else happens land together, and two who connect around the start of a game may not. There is no way yet to choose a game.

## Step 4: fill every place

The supervisor refuses a player only when no game waits with a free seat and its eight places are taken: the player gets `ConnectionRefused` (`GameFull`), which the client shows on its connection screen. `MAXIMUM_RUNNING_GAMES` is a constant (`src/server/SupervisedGames/SupervisedGamesConstants.hpp`), so changing it takes a build; no launch option or line of `server.cfg` changes it.

Each game runs in a process of its own, on its own simulation thread, so a slow game does not slow the others, and an exception a system of one game throws stops only that process: the supervisor writes `game <n> crashed` in its journal, at the `error` level. The journal of each game gives the cost of its ticks every 10 s (see [Metrics](/R-Type/server/#metrics-servermetricsmodule)).

## Step 5: run two supervisors side by side

Several supervisors can run on one machine, each on its own port, with nothing shared between them. Each keeps its leaderboard in a file named after its port, `r-type_server_4251.db` and `r-type_server_4252.db`, and writes the file of its games after its port, `r-type_games_4251.cfg` and `r-type_games_4252.cfg`, next to the executable. Give each its own journal, or their lines mix in one file:

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

Two supervisors know nothing of each other, and a full one sends nobody to the other. Their games still write `r-type_game_0.log`, `r-type_game_1.log` and so on next to the executable, whichever supervisor started them, so the lines of their games mix in the same files: the journal is opened to append. Stop each supervisor with Ctrl-C: it tells its games to stop, which tell their players, writes `server stopped`, and exits with code 0.

## If it does not work

| You see | Cause | Fix |
|---|---|---|
| `Cannot open a UDP socket on 0.0.0.0 port 4242: Address already in use`, exit code 1 | Another supervisor holds the port | Give another port with `-p`, or stop the other one |
| `game 0 crashed while …` in the journal of the supervisor, naming `waves.toml` or `No assets folder found` in `r-type_game_0.log` | `assets/waves/waves.toml` was moved, or the executable was copied away from `assets/`; the supervisor reads neither, so only its games stop | Put them back: see [Writing the waves file](/R-Type/waves-file/) |
| `SERVER UNREACHABLE` on the client after 5 seconds | The supervisor is not running, or the client names another port | Check `listening on UDP port` in the journal, and `-p` on the client |
| `REFUSED: CALLSIGN TAKEN` on the client | Another player of that game holds it, or left less than 20 s ago without saying so | Pick another nickname, or wait for the seat to be freed |
| The lines of two supervisors in one journal | Both write to the default `r-type_server.log` next to the executable, whatever folder they were started in | Give each its own `--log-file` |
