---
title: Writing the waves file
description: How to describe the waves of Bydos of each difficulty in assets/waves/waves.toml, check a design before playing it, and read the error the server prints when the file is wrong.
sidebar:
  label: Waves file
---

The waves file, `assets/waves/waves.toml`, describes the waves of Bydos of each difficulty. This page is for whoever tunes them: it gives the keys, what each one does to a game, how to check a design and how to read the error the server prints, without opening the C++.

Only the server reads the file: the supervisor and each game process, once, when they start, from `waves/waves.toml` in the `assets/` folder next to its executable, or one level above it, where the Visual Studio generator writes `Release/`; a resource may lay a waves file of its own over it (see [Waves of a resource](#waves-of-a-resource)). The folder the server is started from plays no part. The supervisor reads it for the names of its difficulties, which it accepts in a new game and lists to the master; a game process reads it for the waves of the difficulty of its game. A client never reads it; it only draws the Bydos the server announces. The server reads the [entity sheets](/R-Type/entity-sheets/) first, since the file names its enemies among them. An invalid file stops the supervisor when it starts, before it opens any game. A game is played at the difficulty its creator chose, `normal` (`DEFAULT_DIFFICULTY` in `src/identifiers/GameRuleConstants.hpp`) by default; no client screen lets the player choose another one yet. The name of a difficulty is 1 to 16 bytes of printable UTF-8, since the protocol carries it, and a file holds at most `MAXIMUM_DIFFICULTIES` (16). A difficulty may end its level with a boss, the sheet its `boss` key names. The file cannot describe a scripted list of waves or a background per wave: a level a Luau script directs does both, and a game plays one instead of its waves when the setting `level` names it (see [Scripting in Luau](/R-Type/scripting/#directing-a-level)). How the server runs the waves is in [Server and game](/R-Type/server/#waves-of-bydos-wavedirector).

## A difficulty, key by key

A difficulty is a table `[difficulty.<name>]` holding eight keys, a ninth, `boss`, when its level ends with a boss, a tenth, `boss_health_percent`, to make that boss sturdier or frailer, and an eleventh, `look_items`, that may be left out:

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
boss = "bydo_tyrant"
```

| Key | Unit | Accepted | What it changes |
|---|---|---|---|
| `enemies` | names | a list of 1 to 100 names | the entity sheets the Bydos are made from, in turn (see [Enemies](#enemies)) |
| `look_items` | names | a list of 0 to 100 names; may be left out | the look items its waves bring, in turn (see [Look items](#look-items)) |
| `first_wave_bydos` | Bydos | 1 to 100 | how many Bydos the first wave holds |
| `bydos_added_per_wave` | Bydos | 0 to 100 | how many more each next wave holds |
| `first_spawn_interval_ms` | milliseconds | 1 to 60000 | the time between two Bydos of the first wave |
| `spawn_interval_cut_per_wave_ms` | milliseconds | 0 to 60000 | how much closer together the Bydos of each next wave come |
| `shortest_spawn_interval_ms` | milliseconds | 1 to `first_spawn_interval_ms` | the shortest time a wave may leave between two Bydos |
| `wave_pause_ms` | milliseconds | 0 to 60000 | the empty sky before each wave, the first one included |
| `level_waves` | waves | 1 to 100 | how many waves the level holds: without a boss, the game is won once the last one is over (see [How the waves grow](#how-the-waves-grow)) |
| `boss` | a name | the name of an entity sheet with a `[gameplay.boss]` table; may be left out | the boss that arrives once the last wave is over: the game is then won once it is destroyed and no enemy is left (see [The boss](#the-boss)) |
| `boss_health_percent` | percent | 1 to 1000; 100 when left out | the health of its boss, in percent of the health its sheet gives once scaled for the players (see [The boss](#the-boss)) |

Every key but `boss`, `boss_health_percent` and `look_items` is required, and any other key is refused, so a misspelt key stops the server instead of being ignored. Every number is a TOML integer: `2500`, never `2500.0` or `"2500"`. The keys and the ranges live in `src/server/Spawning/WaveFileConstants.hpp`; the names are those of the entity sheets.

## How the waves grow

- Wave n, counted from 1, holds `first_wave_bydos + (n - 1) × bydos_added_per_wave` Bydos, one every `max(first_spawn_interval_ms - (n - 1) × spawn_interval_cut_per_wave_ms, shortest_spawn_interval_ms)` milliseconds. With `normal`, the waves hold 3, 4, 5… Bydos, and from the fifth wave on they come one every second.
- A game starts with a pause of `wave_pause_ms`. The first Bydo of a wave appears at the end of its pause, the next ones an interval apart.
- A wave ends once every one of its Bydos has appeared and none is left in the world, killed or gone off the left edge. The pause before the next wave starts on the tick that finds the sky clear, so a wave killed fast brings the next one sooner, and a Bydo that crosses unharmed makes the players wait for it. A Bydo killed before the last of its wave appeared does not end the wave.
- The level holds `level_waves` waves. Once the last of them ends, no pause and no wave follow. Without a boss the game is won then: the players are told with a `GameEnded` that carries the outcome `Victory` (see [End of a game](/R-Type/server/#end-of-a-game-gameendannouncer)). With a boss, the boss comes first (see [The boss](#the-boss)). A game whose last life is lost on the tick it would be won is lost, not won. With `normal` (8 waves), the last wave holds 10 Bydos.
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

## The boss

`boss` names a sheet that has a `[gameplay.boss]` table (see [Writing an entity sheet](/R-Type/entity-sheets/#gameplay-key-by-key)); the only one shipped is `bydo_tyrant`. On the tick the server finds the last wave over, it warns the players with a `BossStatus` (see [Protocol](/R-Type/protocol/#boss-status)), and the boss appears `BOSS_ARRIVAL_WARNING` (4400 ms) later, off screen past the right edge, on the middle row of the area of play. Its arrival shields it, then it fights. When it appears, its health is the `health.points` of its sheet plus `BOSS_HEALTH_PER_EXTRA_PLAYER` (25 %) for each player connected past the first, then taken at `boss_health_percent`, each step rounded down, and the health it enrages below follows: the Tyrant has 240, 300, 360 and 420 points for one to four players at 100 %, and 312, 390, 468 and 546 in `hard`. The game is won on the first tick after that which finds no enemy in the world: the boss, which never leaves by itself, and the Bydos its volleys brought. How the server runs it is in [Server and game](/R-Type/server/#boss-bossstage-and-bossphasesystem).
## Look items

A name of `look_items` is the name of an entity sheet of side `look_item` (see [Writing an entity sheet](/R-Type/entity-sheets/)): an item of the locker of the hangar that floats to the left and goes to the pilot whose ship touches it first. From the second wave, one item enters at the middle of the right edge as the wave begins, that is when its pause starts, taking the names in turn. It is not a Bydo: it never holds a wave open, and it draws no number from the random generator, so the Bydos of a seed keep their heights. A difficulty without the key, or with an empty list, brings none. The shipped items are `look_party_hat`, `look_crown`, `look_propeller_cap`, `look_cowboy_hat`, `look_viking_helmet`, `look_wizard_hat` and `look_chef_toque`.

## The shipped difficulties

| Key | `easy` | `normal` | `hard` |
|---|---|---|---|
| `enemies` | `drifter`, `drifter`, `weaver` | `drifter`, `weaver`, `drifter`, `voltjelly`, `weaver`, `reaper` | `weaver`, `drifter`, `reaper`, `weaver`, `drillback`, `voltjelly`, `weaver`, `bydomancer` |
| `look_items` | `look_crown`, `look_party_hat`, `look_cowboy_hat`, `look_propeller_cap` | the seven shipped items, `look_crown` first | the seven shipped items, `look_crown` first |
| `first_wave_bydos` | 2 | 3 | 4 |
| `bydos_added_per_wave` | 1 | 1 | 2 |
| `first_spawn_interval_ms` | 2500 | 2000 | 1500 |
| `spawn_interval_cut_per_wave_ms` | 250 | 250 | 250 |
| `shortest_spawn_interval_ms` | 1500 | 1000 | 500 |
| `wave_pause_ms` | 5000 | 4000 | 3000 |
| `level_waves` | 5 | 8 | 10 |
| `boss` | none | `bydo_tyrant` | `bydo_tyrant` |
| `boss_health_percent` | none | none: 100 | 130 |

`easy` leans on the Drifter, which is easier to hit; `normal` adds one Voltjelly and one Reaper in six Bydos; `hard` leans on the Weaver, sends each of the four mobs once in eight Bydos and brings two more Bydos per wave. A wave ends only once its Bydos are gone, so the Voltjelly, twice as slow, makes its wave last longer, and the Reaper and the Bydomancer, which never leave by themselves, hold their wave until they are killed. `easy` ends once its last wave is over; `normal` and `hard` end with the Bydo Tyrant, 30 % sturdier in `hard`. No test pins these values: tuning them needs no rebuild and breaks no test.

## Designing a difficulty

- Start from `normal` and change one key at a time, so the effect of each change is plain.
- If none is killed, up to 8.5 seconds divided by the interval Bydos are on screen at once, and never more than the wave holds: the three Bydos of the first wave of `normal` are all on screen together, and at one Bydo per second, eight or nine are, once the waves hold that many.
- A world state datagram carries 110 changed entities with four players, 114 with one, ships and missiles included (`DeltaWorldStateCodec`). A state lists only the entities that moved since the state the player acknowledged, or all of them after a second without an acknowledgement. Past that, every world state takes one more datagram per player, `--state-rate` times per second (20 by default).
- `bydos_added_per_wave = 0` keeps every wave the size of the first; `shortest_spawn_interval_ms` equal to `first_spawn_interval_ms` keeps their pace.
- Try a change by editing the file and starting the server again; nothing is rebuilt and the client does not change:

```bash
./r-type_server --seed 42
```

Then connect a client. The game process logs, in its journal `r-type_game_<n>.log`, the folder of the entity sheets and their count (`entity sheets /…/assets/entities: 16 types`), then the waves files it read, by their absolute path, the assets first (`waves files /…/assets/waves/waves.toml: 3 difficulties`), the difficulty the games play and whether they are vanilla (`games play normal, not vanilla`, since a seed is set), then each wave at the start of the pause before it (`wave 2: 4 Bydos, one every 1750 ms`). The seed fixes the heights of the Bydos, so the same file and the same seed replay the same game.

## Waves of a resource

A resource started by an `ensure` line may hold its own `waves/waves.toml` (see [Resources](/R-Type/server/#resources-ensure-lines)), which changes only what it names: the server reads the file of the assets, then the file of each resource in the order the resources are laid, and lays each one over the files before it, difficulty by difficulty (`WaveCatalog::fromFiles()`, `src/server/Spawning/`).

- A key the file gives replaces the same key of that difficulty.
- A key it leaves out keeps the value of the files before it, so a file that only gives `boss` changes the boss and nothing else.
- A difficulty only it names is added; the files that name it must give every required key between them.
- A list, `enemies` or `look_items`, is replaced whole, never appended to: give the whole list. `look_items = []` takes the look items away.
- A key cannot be removed: a resource cannot take the boss away from a difficulty of the assets.

Each file is checked on its own for its keys and the names of its difficulties, so a misspelt key names the file that holds it. The values are checked once the files are laid, each at the file and the line it comes from: a `shortest_spawn_interval_ms` of the assets is refused at its own line when a resource lowers `first_spawn_interval_ms` under it. The files together hold at most `MAXIMUM_DIFFICULTIES` (16) difficulties, and the enemies they name may be the sheets the resources bring.

The waves of `resources/crimson-drifter/`, the example the repository ships, give the enemies of normal alone:

```toml
[difficulty.normal]
enemies = ["drifter", "weaver", "crimson_drifter", "voltjelly", "weaver", "reaper", "crimson_drifter"]
```

With it, normal sends two crimson Drifters in seven Bydos and keeps the curve, the look items and the Bydo Tyrant of the assets; easy and hard stay those of the assets.

## The shipped file and vanilla games

A changed waves file, or a waves file a resource lays over it, makes the games of the server not vanilla: the server recognizes the shipped file by its fingerprint, SipHash-2-4 of its bytes, so a comment or a line ending counts as a change (see [Vanilla games](/R-Type/server/#vanilla-games-vanillabreaches)). With a resource, the fingerprint is taken over the files joined by a NUL byte (`WAVE_FILE_SEPARATOR`), which no TOML text holds. Their scores stay on the leaderboard of that server. `.gitattributes` keeps the file in LF on every checkout, Windows included.

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
| a look item that is a Bydo (`"weaver"` in `look_items`) | `…/waves.toml:<line>:<column>: difficulty.normal.look_items: unknown look item "weaver"; known: look_chef_toque, look_cowboy_hat, …` |
| a list of more than 100 look items | `…/waves.toml:<line>:<column>: difficulty.normal.look_items must be a list of 0 to 100 look item names` |
| an enemy the server has no sheet for (`"bos"`) | `…/waves.toml:37:23: difficulty.normal.enemies: unknown enemy "bos"; known: drifter, weaver` |
| a sheet that is not of side `enemy` (`"ship"` in `easy`) | `…/waves.toml:30:12: difficulty.easy.enemies: unknown enemy "ship"; known: drifter, weaver` |
| no `normal` difficulty | `no difficulty normal in the waves file, which describes: easy, hard, medium` |
| nothing: the file is missing | `cannot read the file /…/assets/waves/waves.toml` |
| nothing: no `assets/` folder next to the executable or above it | `No assets folder found, searched: /…/bin/assets, /…/assets` |

`level_waves` follows the same messages: a difficulty without it stops the server with `difficulty.normal.level_waves is missing`, and a value outside 1 to 100, or one that is not an integer, with `difficulty.normal.level_waves must be an integer from 1 to 100`. A file written before the key existed is refused the same way, so add `level_waves` to every difficulty of a file of your own.

A `boss` that names no sheet, or a sheet without `[gameplay.boss]`, stops the server with the name and the bosses it knows: `difficulty.normal.boss: unknown boss "drifter"; known: bydo_tyrant`, after the place of the value.

Neither of the last two depends on the folder the server is started from. No assets folder means the executable was copied away from the repository; a missing file means `assets/waves/waves.toml` was moved or renamed.

## Adding a kind of enemy

A new enemy is a new sheet of side `enemy` in `assets/entities/`, with its behaviour, its points and its look (see [Writing an entity sheet](/R-Type/entity-sheets/)); the file can name it at once, and nothing is rebuilt unless it needs a new behaviour. The server tells each player the name of every sheet, and the client draws each enemy with the `[appearance]` of its own sheet of that name, so a kind may look different without a change of the protocol. The [tutorial](/R-Type/tutorial-add-an-enemy-type/) adds one step by step.
