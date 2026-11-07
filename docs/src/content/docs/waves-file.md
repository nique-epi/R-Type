---
title: Writing the waves file
description: How to describe the waves of Bydos of each difficulty in assets/waves/waves.toml, check a design before playing it, and read the error the server prints when the file is wrong.
sidebar:
  label: Waves file
---

The waves file, `assets/waves/waves.toml`, describes the waves of Bydos of each difficulty. This page is for whoever tunes them: it gives the keys, what each one does to a game, how to check a design and how to read the error the server prints, without opening the C++.

Only the server reads the file: each game process, once, when it starts, from `waves/waves.toml` in the `assets/` folder next to its executable, or one level above it, where the Visual Studio generator writes `Release/`. The folder the server is started from plays no part. The supervisor never reads it, so a game process starts, and reads the file, when the first player who needs a game connects. A client never reads it; it only draws the Bydos the server announces. The server reads the [entity sheets](/R-Type/entity-sheets/) first, since the file names its enemies among them. An invalid file stops each game process when it starts, before its game exists, and the supervisor writes in its journal that the game crashed; the supervisor itself keeps running. Every game plays the difficulty `normal` (`DEFAULT_DIFFICULTY` in `src/server/ServerConstants.hpp`): the other difficulties are read and checked at startup, but nothing lets the players choose one yet. The file cannot describe a scripted list of waves, a background per wave or a boss: none of these exists yet. How the server runs the waves is in [Server and game](/R-Type/server/#waves-of-bydos-wavedirector).

## A difficulty, key by key

A difficulty is a table `[difficulty.<name>]` holding eight keys:

```toml
[difficulty.normal]
enemies = ["drifter", "weaver"]
first_wave_bydos = 3
bydos_added_per_wave = 1
first_spawn_interval_ms = 2000
spawn_interval_cut_per_wave_ms = 250
shortest_spawn_interval_ms = 1000
wave_pause_ms = 4000
level_waves = 8
```

| Key | Unit | Accepted | What it changes |
|---|---|---|---|
| `enemies` | names | a list of 1 to 100 names | the entity sheets the Bydos are made from, in turn (see [Enemies](#enemies)) |
| `first_wave_bydos` | Bydos | 1 to 100 | how many Bydos the first wave holds |
| `bydos_added_per_wave` | Bydos | 0 to 100 | how many more each next wave holds |
| `first_spawn_interval_ms` | milliseconds | 1 to 60000 | the time between two Bydos of the first wave |
| `spawn_interval_cut_per_wave_ms` | milliseconds | 0 to 60000 | how much closer together the Bydos of each next wave come |
| `shortest_spawn_interval_ms` | milliseconds | 1 to `first_spawn_interval_ms` | the shortest time a wave may leave between two Bydos |
| `wave_pause_ms` | milliseconds | 0 to 60000 | the empty sky before each wave, the first one included |
| `level_waves` | waves | 1 to 100 | how many waves the level holds: the game is won once the last one is over (see [How the waves grow](#how-the-waves-grow)) |

Every key is required, and any other key is refused, so a misspelt key stops the server instead of being ignored. Every number is a TOML integer: `2500`, never `2500.0` or `"2500"`. The keys and the ranges live in `src/server/Spawning/WaveFileConstants.hpp`; the names are those of the entity sheets.

## How the waves grow

- Wave n, counted from 1, holds `first_wave_bydos + (n - 1) × bydos_added_per_wave` Bydos, one every `max(first_spawn_interval_ms - (n - 1) × spawn_interval_cut_per_wave_ms, shortest_spawn_interval_ms)` milliseconds. With `normal`, the waves hold 3, 4, 5… Bydos, and from the fifth wave on they come one every second.
- A game starts with a pause of `wave_pause_ms`. The first Bydo of a wave appears at the end of its pause, the next ones an interval apart.
- A wave ends once every one of its Bydos has appeared and none is left in the world, killed or gone off the left edge. The pause before the next wave starts on the tick that finds the sky clear, so a wave killed fast brings the next one sooner, and a Bydo that crosses unharmed makes the players wait for it. A Bydo killed before the last of its wave appeared does not end the wave.
- The level holds `level_waves` waves. Once the last of them ends, no pause and no wave follow, and the game is won: the players are told with a `GameEnded` that carries the outcome `Victory` (see [End of a game](/R-Type/server/#end-of-a-game-gameendannouncer)). A game whose last life is lost on the same tick is lost, not won. With `normal` (8 waves), the last wave holds 10 Bydos.
- A Bydo crosses the playfield in about 8.5 seconds: 2048 logical units at the `top_speed` of the Drifter and the Weaver (240 units per second), from `ENEMY_SPAWN_MARGIN` (64) past the right edge to `ENEMY_EXIT_MARGIN` (64) past the left one.
- Time is counted in ticks of a sixtieth of a second, 16 666 666 ns each, so a duration ends on the tick that reaches it: a pause of 4000 ms ends on its 241st tick.

## Enemies

A name of the list is the name of an entity sheet of side `enemy` in `assets/entities/` (see [Writing an entity sheet](/R-Type/entity-sheets/)). The shipped ones:

| Name | Sheet | On screen | Points |
|---|---|---|---|
| `drifter` | `assets/entities/drifter.toml` | goes straight | 100 |
| `weaver` | `assets/entities/weaver.toml` | weaves up and down, 48 logical units each way, every 2 seconds | 200 |
| `voltjelly` | `assets/entities/voltjelly.toml` | drifts with the sky, half as fast as a Bydo, inside an aura that kills a ship that touches it; it fires nothing | 100 |
| `reaper` | `assets/entities/reaper.toml` | comes to the height of the nearest ship and swings a scythe at it | 200 |
| `drillback` | `assets/entities/drillback.toml` | drifts, shakes, then charges straight ahead; one shot kills it while it is stunned | 300 |
| `bydomancer` | `assets/entities/bydomancer.toml` | stands at the back and casts rune orbs that weave and burst, and a nova at a ship that comes close | 300 |

The Bydos take the names of the list in turn, and the turn carries on from one wave to the next. With `["drifter", "weaver"]` and three Bydos in the first wave, wave 1 brings a Drifter, a Weaver and a Drifter, and wave 2 starts with a Weaver. A name may appear several times to weight a kind: `["drifter", "drifter", "weaver"]` brings two Drifters for each Weaver.

## The shipped difficulties

| Key | `easy` | `normal` | `hard` |
|---|---|---|---|
| `enemies` | `drifter`, `drifter`, `weaver` | `drifter`, `weaver`, `drifter`, `voltjelly`, `weaver`, `reaper` | `weaver`, `drifter`, `reaper`, `weaver`, `drillback`, `voltjelly`, `weaver`, `bydomancer` |
| `first_wave_bydos` | 2 | 3 | 4 |
| `bydos_added_per_wave` | 1 | 1 | 2 |
| `first_spawn_interval_ms` | 2500 | 2000 | 1500 |
| `spawn_interval_cut_per_wave_ms` | 250 | 250 | 250 |
| `shortest_spawn_interval_ms` | 1500 | 1000 | 500 |
| `wave_pause_ms` | 5000 | 4000 | 3000 |
| `level_waves` | 5 | 8 | 10 |

`easy` leans on the Drifter, which is easier to hit; `normal` adds one Voltjelly and one Reaper in six Bydos; `hard` leans on the Weaver, sends each of the four mobs once in eight Bydos and brings two more Bydos per wave. A wave ends only once its Bydos are gone, so the Voltjelly, twice as slow, makes its wave last longer, and the Reaper and the Bydomancer, which never leave by themselves, hold their wave until they are killed. No test pins these values: tuning them needs no rebuild and breaks no test.

## Designing a difficulty

- Start from `normal` and change one key at a time, so the effect of each change is plain.
- If none is killed, up to 8.5 seconds divided by the interval Bydos are on screen at once, and never more than the wave holds: the three Bydos of the first wave of `normal` are all on screen together, and at one Bydo per second, eight or nine are, once the waves hold that many.
- A world state datagram carries 122 changed entities with four players, 127 with one, ships and missiles included (`DeltaWorldStateCodec`). A state lists only the entities that moved since the state the player acknowledged, or all of them after a second without an acknowledgement. Past that, every world state takes one more datagram per player, `--state-rate` times per second (20 by default).
- `bydos_added_per_wave = 0` keeps every wave the size of the first; `shortest_spawn_interval_ms` equal to `first_spawn_interval_ms` keeps their pace.
- Try a change by editing the file and starting the server again; nothing is rebuilt and the client does not change:

```bash
./r-type_server --seed 42
```

Then connect a client. The game process logs, in its journal `r-type_game_<n>.log`, the folder of the entity sheets and their count (`entity sheets /…/assets/entities: 5 types`), then the file it read, by its absolute path (`waves file /…/assets/waves/waves.toml: 3 difficulties`), the difficulty the games play and whether they are vanilla (`games play normal, not vanilla`, since a seed is set), then each wave at the start of the pause before it (`wave 2: 4 Bydos, one every 1750 ms`). The seed fixes the heights of the Bydos, so the same file and the same seed replay the same game.

## The shipped file and vanilla games

A resource started by an `ensure` line may hold its own `waves/waves.toml`: the server then reads the waves of the last resource laid that holds one, instead of those of the assets, and the enemies it names may be the sheets the resource brings (see [Resources](/R-Type/server/#resources-ensure-lines)). The file is read with the same rules.

A changed waves file makes the games of the server not vanilla: the server recognizes the shipped file by its fingerprint, SipHash-2-4 of its bytes, so a comment or a line ending counts as a change (see [Vanilla games](/R-Type/server/#vanilla-games-vanillabreaches)). Their scores stay on the leaderboard of that server. `.gitattributes` keeps the file in LF on every checkout, Windows included.

When the shipped file itself changes, `OFFICIAL_WAVES_FINGERPRINT` in `src/server/ServerConstants.hpp` changes with it: `WaveCatalog.TheShippedFileHasTheOfficialFingerprint` fails until it does, and its failure prints, in decimal, the fingerprint the file now has.

## When the server refuses the file

A game process reads the whole file, every difficulty included, when it starts, before its game exists. On the first error it prints `game stopped: The module "games" failed to start:`, in its journal too, the absolute path of the file, the line and the column of what is wrong, the dotted name of the key and the rule it breaks, then exits with code 1. The messages below come from runs of the server; the path is shortened to `…/waves.toml`.

| The file holds… | The server prints |
|---|---|
| text that is not TOML (`wave_pause_ms = 4000 ms`) | `…/waves.toml:43:22: Error while parsing key-value pair: expected a comment or whitespace, saw 'm'` |
| a misspelt key (`frist_wave_bydos`) | `…/waves.toml:47:1: unknown key difficulty.hard.frist_wave_bydos` |
| a key less | `…/waves.toml:36:1: difficulty.normal.wave_pause_ms is missing`, at the line of the table |
| a value out of its range (`first_wave_bydos = 0`) | `…/waves.toml:38:20: difficulty.normal.first_wave_bydos must be an integer from 1 to 100` |
| a number that is not an integer (`2000.0`) | `…/waves.toml:40:27: difficulty.normal.first_spawn_interval_ms must be an integer from 1 to 60000` |
| an empty list of enemies | `…/waves.toml:37:11: difficulty.normal.enemies must be a list of 1 to 100 enemy names` |
| an enemy the server has no sheet for (`"bos"`) | `…/waves.toml:37:23: difficulty.normal.enemies: unknown enemy "bos"; known: drifter, weaver` |
| a sheet that is not of side `enemy` (`"ship"` in `easy`) | `…/waves.toml:30:12: difficulty.easy.enemies: unknown enemy "ship"; known: drifter, weaver` |
| no `normal` difficulty | `no difficulty normal in the waves file, which describes: easy, hard, medium` |
| nothing: the file is missing | `cannot read the file /…/assets/waves/waves.toml` |
| nothing: no `assets/` folder next to the executable or above it | `No assets folder found, searched: /…/bin/assets, /…/assets` |

`level_waves` follows the same messages: a difficulty without it stops the server with `difficulty.normal.level_waves is missing`, and a value outside 1 to 100, or one that is not an integer, with `difficulty.normal.level_waves must be an integer from 1 to 100`. A file written before the key existed is refused the same way, so add `level_waves` to every difficulty of a file of your own.

Neither of the last two depends on the folder the server is started from. No assets folder means the executable was copied away from the repository; a missing file means `assets/waves/waves.toml` was moved or renamed.

## Adding a kind of enemy

A new enemy is a new sheet of side `enemy` in `assets/entities/`, with its behaviour, its points and its look (see [Writing an entity sheet](/R-Type/entity-sheets/)); the file can name it at once, and nothing is rebuilt unless it needs a new behaviour. The server tells each player the name of every sheet, and the client draws each enemy with the `[appearance]` of its own sheet of that name, so a kind may look different without a change of the protocol. The [tutorial](/R-Type/tutorial-add-an-enemy-type/) adds one step by step.
