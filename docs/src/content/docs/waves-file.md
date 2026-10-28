---
title: Writing the waves file
description: How to describe the waves of Bydos of each difficulty in assets/waves/waves.toml, check a design before playing it, and read the error the server prints when the file is wrong.
sidebar:
  label: Waves file
---

The waves file, `assets/waves/waves.toml`, describes the waves of Bydos of each difficulty. This page is for whoever tunes them: it gives the keys, what each one does to a game, how to check a design and how to read the error the server prints, without opening the C++.

Only the server reads the file, once, when it starts, from `assets/waves/waves.toml` in the folder it is started in: the repository root, where the build writes `r-type_server`. A client never reads it; it only draws the Bydos the server announces. An invalid file stops the server before it listens. Every game plays the difficulty `normal` (`DEFAULT_DIFFICULTY` in `src/server/ServerConstants.hpp`): the other difficulties are read and checked at startup, but nothing lets the players choose one yet. The file cannot describe a scripted list of waves, a background per wave or a boss: none of these exists yet. How the server runs the waves is in [Server and game](/R-Type/server/#waves-of-bydos-wavedirector).

## A difficulty, key by key

A difficulty is a table `[difficulty.<name>]` holding seven keys:

```toml
[difficulty.normal]
enemies = ["drifter", "weaver"]
first_wave_bydos = 3
bydos_added_per_wave = 1
first_spawn_interval_ms = 2000
spawn_interval_cut_per_wave_ms = 250
shortest_spawn_interval_ms = 1000
wave_pause_ms = 4000
```

| Key | Unit | Accepted | What it changes |
|---|---|---|---|
| `enemies` | names | a list of 1 to 100 names | the kinds the Bydos take, in turn (see [Enemies](#enemies)) |
| `first_wave_bydos` | Bydos | 1 to 100 | how many Bydos the first wave holds |
| `bydos_added_per_wave` | Bydos | 0 to 100 | how many more each next wave holds |
| `first_spawn_interval_ms` | milliseconds | 1 to 60000 | the time between two Bydos of the first wave |
| `spawn_interval_cut_per_wave_ms` | milliseconds | 0 to 60000 | how much closer together the Bydos of each next wave come |
| `shortest_spawn_interval_ms` | milliseconds | 1 to `first_spawn_interval_ms` | the shortest time a wave may leave between two Bydos |
| `wave_pause_ms` | milliseconds | 0 to 60000 | the empty sky before each wave, the first one included |

Every key is required, and any other key is refused, so a misspelt key stops the server instead of being ignored. Every number is a TOML integer: `2500`, never `2500.0` or `"2500"`. The keys, the ranges and the names live in `src/server/Spawning/WaveFileConstants.hpp`.

## How the waves grow

- Wave n, counted from 1, holds `first_wave_bydos + (n - 1) × bydos_added_per_wave` Bydos, one every `max(first_spawn_interval_ms - (n - 1) × spawn_interval_cut_per_wave_ms, shortest_spawn_interval_ms)` milliseconds. With `normal`, the waves hold 3, 4, 5… Bydos, and from the fifth wave on they come one every second.
- A game starts with a pause of `wave_pause_ms`. The first Bydo of a wave appears at the end of its pause, the next ones an interval apart.
- A wave ends once every one of its Bydos has appeared and none is left in the world, killed or gone off the left edge. The pause before the next wave starts on the tick that finds the sky clear, so a wave killed fast brings the next one sooner, and a Bydo that crosses unharmed makes the players wait for it. A Bydo killed before the last of its wave appeared does not end the wave.
- A Bydo crosses the playfield in about 8.5 seconds: 2048 logical units at `BYDO_SPEED` (240 units per second), from `BYDO_SPAWN_MARGIN` (64) past the right edge to `BYDO_EXIT_MARGIN` (64) past the left one.
- Time is counted in ticks of a sixtieth of a second, 16 666 666 ns each, so a duration ends on the tick that reaches it: a pause of 4000 ms ends on its 241st tick.

## Enemies

| Name | Kind | On screen | Points |
|---|---|---|---|
| `drifter` | `BydoKind::Drifter` | goes straight | 100 |
| `weaver` | `BydoKind::Weaver` | weaves up and down, 48 logical units each way, every 2 seconds | 200 |

The Bydos take the names of the list in turn, and the turn carries on from one wave to the next. With `["drifter", "weaver"]` and three Bydos in the first wave, wave 1 brings a Drifter, a Weaver and a Drifter, and wave 2 starts with a Weaver. A name may appear several times to weight a kind: `["drifter", "drifter", "weaver"]` brings two Drifters for each Weaver.

## The shipped difficulties

| Key | `easy` | `normal` | `hard` |
|---|---|---|---|
| `enemies` | `drifter`, `drifter`, `weaver` | `drifter`, `weaver` | `weaver`, `weaver`, `drifter` |
| `first_wave_bydos` | 2 | 3 | 4 |
| `bydos_added_per_wave` | 1 | 1 | 2 |
| `first_spawn_interval_ms` | 2500 | 2000 | 1500 |
| `spawn_interval_cut_per_wave_ms` | 250 | 250 | 250 |
| `shortest_spawn_interval_ms` | 1500 | 1000 | 500 |
| `wave_pause_ms` | 5000 | 4000 | 3000 |

`easy` leans on the Drifter, which is easier to hit; `hard` leans on the Weaver and brings two more Bydos per wave. No test pins these values: tuning them needs no rebuild and breaks no test.

## Designing a difficulty

- Start from `normal` and change one key at a time, so the effect of each change is plain.
- If none is killed, up to 8.5 seconds divided by the interval Bydos are on screen at once, and never more than the wave holds: the three Bydos of the first wave of `normal` are all on screen together, and at one Bydo per second, eight or nine are, once the waves hold that many.
- A world state datagram carries 124 entities with four players, 128 with one, ships and missiles included (`WorldStateCodec`). Past that, every world state takes one more datagram per player, `WORLD_STATES_PER_SECOND` (20) times per second.
- `bydos_added_per_wave = 0` keeps every wave the size of the first; `shortest_spawn_interval_ms` equal to `first_spawn_interval_ms` keeps their pace.
- Try a change by editing the file and starting the server again from the repository root; nothing is rebuilt and the client does not change:

```bash
./r-type_server --seed 42
```

The server logs the file it read (`waves file assets/waves/waves.toml: 3 difficulties, games play normal`), then each wave at the start of the pause before it (`wave 2: 4 Bydos, one every 1750 ms`). The seed fixes the heights of the Bydos, so the same file and the same seed replay the same game.

## When the server refuses the file

The server reads the whole file, every difficulty included, before it listens. On the first error it prints `server stopped:`, the absolute path of the file, the line and the column of what is wrong, the dotted name of the key and the rule it breaks, then exits with code 1. The messages below come from runs of the server; the path is shortened to `…/waves.toml`.

| The file holds… | The server prints |
|---|---|
| text that is not TOML (`wave_pause_ms = 4000 ms`) | `…/waves.toml:43:22: Error while parsing key-value pair: expected a comment or whitespace, saw 'm'` |
| a misspelt key (`frist_wave_bydos`) | `…/waves.toml:47:1: unknown key difficulty.hard.frist_wave_bydos` |
| a key less | `…/waves.toml:36:1: difficulty.normal.wave_pause_ms is missing`, at the line of the table |
| a value out of its range (`first_wave_bydos = 0`) | `…/waves.toml:38:20: difficulty.normal.first_wave_bydos must be an integer from 1 to 100` |
| a number that is not an integer (`2000.0`) | `…/waves.toml:40:27: difficulty.normal.first_spawn_interval_ms must be an integer from 1 to 60000` |
| an empty list of enemies | `…/waves.toml:37:11: difficulty.normal.enemies must be a list of 1 to 100 enemy names` |
| an enemy the server does not know (`"bos"`) | `…/waves.toml:37:23: difficulty.normal.enemies: unknown enemy "bos"; known: drifter, weaver` |
| no `normal` difficulty | `no difficulty normal in the waves file, which describes: easy, hard, medium` |
| nothing: the file is missing | `cannot read the waves file /…/assets/waves/waves.toml` |

A missing file usually means the server was started from another folder: start it from the repository root.

## Adding a kind of enemy

A new kind takes a value of `BydoKind` with its pattern (`patternShapeOf`) and its points (`pointsOf`) in `src/game/Spawning/BydoKind/`, then a name in `ENEMY_NAMES` in `src/server/Spawning/WaveFileConstants.hpp`; the file can name it at once. Every Bydo goes over the network as a `Bydo` today, so the client draws every kind with the same sprite: a kind that must look different also needs a network entity type of its own and a sprite on the client.
