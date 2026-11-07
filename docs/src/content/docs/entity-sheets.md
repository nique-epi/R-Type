---
title: Writing an entity sheet
description: How to describe a type of entity in assets/entities/, the keys of its gameplay and of its appearance, the rules between them, and the error a program prints when a sheet is wrong.
sidebar:
  label: Entity sheets
---

An entity sheet, `assets/entities/<name>.toml`, describes one type of entity: the ship of a player, a Bydo, a missile. This page is for whoever adds or tunes a type: it gives the keys, what each one gives an entity, the rules between keys and between sheets, and how to read the error a program prints, without opening the C++. Why the types are data and not code is told in [Architecture](/R-Type/architecture/#entity-types).

Both programs read the folder `entities` (`ENTITY_SHEET_FOLDER_IN_ASSETS`) of the `assets/` folder next to their executable, or one level above it, once, when they start; the folder they are started from plays no part. On the server, each game process reads the table `[gameplay]` of each sheet (`EntitySheets`, `src/game/EntityTypes/`) when its games module starts, so when the first player who needs that game connects; the supervisor reads no sheet, before the waves file, which names its enemies among the sheets. The client reads the table `[appearance]` (`EntityAppearances`, `src/client/Rendering/Sprites/EntityAppearances/`) when its assets module is installed, before it opens its window. Each checks that the root of a sheet holds exactly these two tables, and only the keys of its own: the server never reads `[appearance]`, the client never reads `[gameplay]`.

## The folder and the names

- Every `.toml` file of the folder is a sheet; any other file, and every sub-folder, is left alone. The name of the file without `.toml` is the name of the type: `weaver.toml` is `weaver`.
- A name is 1 to 24 bytes (`MAXIMUM_ENTITY_TYPE_NAME_LENGTH`) of lowercase letters `a` to `z`, digits and `_`, so a file has the same name on every system. A folder holds at most 32 sheets (`MAXIMUM_ENTITY_TYPES`) and at least one. Both limits and the check of a name (`isEntityTypeName()`) live in `src/identifiers/`, so a name the game accepts is one the table of the types can carry.
- The server numbers the sheets from 1 in the order of their names. The eleven sheets the game ships are `bydomancer` (1), `drifter` (2), `drillback` (3), `enemy_missile` (4), `orb_shard` (5), `player_missile` (6), `reaper` (7), `rune_orb` (8), `ship` (9), `voltjelly` (10) and `weaver` (11). These numbers name the types on the wire for as long as the server runs, and every player learns them when it is accepted (see [Protocol](/R-Type/protocol/#entity-types)), so a new sheet whose name sorts first renumbers the others without breaking anything.
- The server makes the ship of every player from the sheet named `ship` (`SHIP_SHEET_NAME`, `src/server/ServerConstants.hpp`): without it, it stops before its first game.

## `[gameplay]`, key by key

The gameplay of the Drifter, from `assets/entities/drifter.toml`:

```toml
[gameplay]
side = "enemy"
pilot = "straight"

[gameplay.hitbox]
width = 96
height = 96

[gameplay.motion]
kind = "steady"
top_speed = 240

[gameplay.behaviour]
heading = -1

[gameplay.health]
points = 3

[gameplay.score]
points = 100

[gameplay.gun]
interval_ms = 1500
first_shot_ms = 1500
projectile = "enemy_missile"
```

`World::spawn(sheet, position, owner)` (`src/game/World.hpp`) makes every entity of the game from its sheet, with the components below, an `Intent` and a `NetworkIdentity`. A body no behaviour pilots is the ship of a player: it also gets `STARTING_LIVES` (3) lives.

| Key | Unit | Accepted | What it gives the entity |
|---|---|---|---|
| `side` | | `player`, `enemy`, `player_missile`, `enemy_missile` | its `EntityType`: what it hits and what hits it (see [Collisions](/R-Type/server/#rules-collisionrules)), and the layer the client draws it in |
| `pilot` | | `player`, or the name of a behaviour: `straight`, `weave`, `scatter`, `reaper`, `drillback`, `bydomancer` | what writes its `Intent`: the input of its player, or that behaviour (see [Behaviours](#behaviours-and-their-settings)) |
| `hitbox.width`, `hitbox.height` | logical units | 1 to 1024 | its `CollisionBox`, centered on its position |
| `motion.kind` | | `ship`, `steady` | how a direction moves it: the drift, roll and dash of a ship (`ShipMotion`), or its top speed times the direction (`SteadyMotion`) |
| `motion.top_speed` | logical units per second | 1 to 4096; for `steady` only | its speed on each axis at a full direction |
| `[gameplay.behaviour]` | | the settings its behaviour declares | the values its behaviour reads |
| `health.points` | points | 1 to 1000 | its `Health`: each missile that hits it takes `MISSILE_DAMAGE` (1) |
| `score.points` | points | 0 to 1000000 | its `ScoreValue`: what the player whose missile destroys it earns |
| `gun.interval_ms` | milliseconds | 1 to 60000 | the time between two shots of its `Gun` |
| `gun.first_shot_ms` | milliseconds | 0 to 60000 | the time before its first shot: 0 for a gun ready at once |
| `gun.projectile` | | the name of a sheet | what its gun fires |
| `hurt_zone.box` | | an array of tables, `[[gameplay.hurt_zone.box]]`, at least one; enemies only | its `HurtZone`: rectangles that hurt a ship that touches them, besides its hitbox |
| `hurt_zone.box.offset_x`, `offset_y` | logical units | -1024 to 1024 | where the center of the box lies from the position of the body |
| `hurt_zone.box.width`, `height` | logical units | 1 to 1024 | the size of the box |
| `hurt_zone.box.attack`, `phase` | | `attack`: 0 to 255; `phase`: `signal`, `strike`, `recover`, `enraged` | the attack and the phase the box hurts in: given together, or not at all for a box that always hurts |
| `hurt_zone.box.from_ms`, `until_ms` | milliseconds | 0 to 60000; only with `attack` and `phase` | when the box hurts within its phase: from `from_ms` into it (0 when left out) until `until_ms`, excluded (the end of the phase when left out), so a sheet gives each frame of a strike its own box |
| `[gameplay.survives_contact]` | | an empty table; enemies only | the `SurvivesContact` marker: a ship that touches the body loses a life, and the body stays |
| `fragile.attack`, `fragile.phase` | | as for a box, both required; enemies only | its `Fragile`: any hit while it is in that phase of that attack takes every point it has left, as the Drillback stunned |
| `burst.after_ms` | milliseconds | 1 to 60000; missiles only | its `ShardBurst`: how long it lives before it bursts |
| `burst.count` | shards | 1 to 8 | how many shards it bursts into, spread evenly around it from the diagonal up and to the right: 4 fly along the four diagonals |
| `burst.shard` | | the name of a sheet of the same side | what it bursts into |

- `side`, `pilot`, `hitbox` and `motion` are required; the tables `behaviour`, `health`, `score`, `gun`, `hurt_zone`, `survives_contact`, `fragile` and `burst` may be left out. A body without `health` takes no damage: a missile that hits it still disappears, and a ship pays a hit with a life instead (see [Lives](/R-Type/server/#lives-lifekeeper)). A body without `score` earns nothing, without `gun` fires nothing.
- Any other key is refused, so a misspelt key stops the program instead of being ignored. Every number is a TOML integer: `240`, never `240.0` or `"240"`.
- A duration of the gun becomes the nearest whole number of ticks of a sixtieth of a second: 333 ms are 20 ticks, 1500 ms are 90. The interval is at least 1 tick (`MINIMUM_GUN_INTERVAL_TICKS`), and so is the time before a burst (`MINIMUM_BURST_TICKS`).
- The keys, the choices and the ranges live in `src/game/EntityTypes/EntitySheetConstants.hpp`.

## Rules between keys and between sheets

A sheet whose keys are each valid is refused when it does not hold together:

- A `player` pilot steers a body of side `player`, and only such a body.
- A body of side `player` moves as a `ship`, and only such a body. Its hitbox is 128 × 64 (`SHIP_WIDTH` × `SHIP_HEIGHT`, `src/game/ShipControl/ShipControlConstants.hpp`), the box `shipAfter()` keeps on screen.
- A missile, of side `player_missile` or `enemy_missile`, has no gun.
- A missile flies towards the edge that removes it (see [Piloted bodies](/R-Type/server/#piloted-bodies-pilotsystem-and-playfieldexitsystem)): on its first tick its pilot steers a missile of side `player_missile` to the right and one of side `enemy_missile` to the left. A `straight` heading of 1 for the first, of -1 for the second; the `weave` goes left, and so does `scatter` on its first tick, still on the point it steers from, so both may pilot a missile of an enemy only. A missile flying the other way, or staying, would never be removed when it hits nothing. The check looks at the first tick only: a missile that turns later, as a shard of a burst, leaves by any edge it flies towards.
- `[gameplay.behaviour]` stands only when the behaviour of the pilot declares settings, and then holds exactly those settings.
- A gun fires a sheet of the folder: of side `player_missile` for a shooter of side `player`, of side `enemy_missile` for any other shooter.
- Only a body of side `enemy` has a `hurt_zone`, `survives_contact` or `fragile`, and a `hurt_zone` holds at least one box.
- Only a missile has a `burst`, and it bursts into missiles of its own side, of a sheet of the folder.
- A box names its `attack` and its `phase` together, or neither; a box that names neither has no `from_ms` or `until_ms`, and `until_ms` is above `from_ms`. An `attack` is a number the sheet gives its own attacks: the code knows no attack by name.
- The waves file names enemies among the sheets of side `enemy` (see [Writing the waves file](/R-Type/waves-file/#enemies)).

## Behaviours and their settings

A behaviour is a named piece of C++ in `src/game/Behaviours/Movements/` (those that only move in `Paths/`, those that attack in `Attacks/`) that writes the `Intent` of its body once per tick (see [Architecture](/R-Type/architecture/#pilot-and-body)). It declares the settings it reads, and the reader of the sheets checks them, so a misspelt setting or a value out of its range stops the program instead of running with a default.

| Behaviour | Moves its body | Settings |
|---|---|---|
| `straight` | along its row, at its top speed, the way its heading says | `heading`: -1 to the left, 1 to the right, 0 to stay |
| `weave` | to the left at its top speed, `amplitude` × sin(2π × age ÷ `period_ms`) below its row at the end of each tick | `amplitude`: 0 to 48 logical units (`ENEMY_PATTERN_ROOM`, the room every row of the waves keeps above and below it); `period_ms`: 1 to 60000 milliseconds |
| `scatter` | straight away from the point it steers from, at its top speed; to the left while still on that point | none |
| `reaper` | to the point `reach` logical units to the right of the nearest ship, at its height; to the left without a ship. Within `align` units of that point on each axis and rested, it stops and swings attack 0 | `reach`, `align`, `rest_ms`, `signal_ms`, `strike_ms`, `recover_ms` |
| `drillback` | to the left at `drift_speed`; left of `enter_x` and rested, it shakes (attack 0, signal), charges left at its top speed (strike), then is stunned (recover) | `drift_speed`, `enter_x`, `rest_ms`, `signal_ms`, `strike_ms`, `recover_ms` |
| `bydomancer` | to the left until `stand_x`, then bobbing `bob` units around its row every `bob_period_ms`; once wounded it leaps `jump` units towards the middle of the area of play, once. Rested at its column it casts its orb (attack 0), and a ship within `nova_reach` makes it cast its nova (attack 1, a strike of `nova_ms`), once idle `nova_rest_ms` | `stand_x`, `jump`, `bob`, `bob_period_ms`, `rest_ms`, `signal_ms`, `strike_ms`, `recover_ms`, `nova_reach`, `nova_ms`, `nova_rest_ms` |

`straight` and `weave` want to fire at every tick, so a body fires as often as its gun allows; a body without a gun fires nothing. `scatter`, `reaper` and `drillback` never fire; `bydomancer` wants to fire during the strike of its orb only, so its gun, ready at once and waiting longer than a strike, fires one orb a cast. The settings of an attack are milliseconds (`rest_ms` 0 to 60000, `signal_ms` and `strike_ms` 1 to 60000, `recover_ms` 0 to 60000, 0 skipping the recover); the others are logical units. A behaviour that attacks steps through its phases with `ActionCycle` and starts an attack again once it has been idle `rest_ms` (see [Piloted bodies](/R-Type/server/#piloted-bodies-pilotsystem-and-playfieldexitsystem)). A weave needs a top speed of at least 2π × `amplitude` ÷ `period_ms`, 151 logical units per second for the Weaver: a slower body falls behind its weave. A new behaviour is a class implementing `IBehaviour`, added to `builtInBehaviours()`, as the [tutorial](/R-Type/tutorial-add-an-enemy-type/) shows.

## `[appearance]`, key by key

The appearance of the Drifter:

```toml
[appearance]
sheet = "sprites/entities.png"

[appearance.idle]
left = 0
top = 72
width = 24
height = 24
frames = 2
spacing = 2
frame_ms = 250
```

| Key | Unit | Accepted | What it gives the entity |
|---|---|---|---|
| `sheet` | | a text: the asset id of a sprite sheet | the texture it is drawn from, which the game screen loads with its other assets |
| `idle.left`, `idle.top` | pixels of the sheet | 0 to 4095 | the top-left corner of its first frame |
| `idle.width`, `idle.height` | pixels of the sheet | 1 to 4096 | the size of a frame |
| `idle.frames` | frames | 1 to 16 | how many frames follow each other in the row |
| `idle.spacing` | pixels of the sheet | 0 to 64 | the gap between two frames |
| `idle.frame_ms` | milliseconds | 1 to 60000 | how long each frame shows, in a loop |

Every key is required and any other is refused. The keys and ranges live in `src/client/Rendering/Sprites/EntityAppearanceConstants.hpp`. A pixel of a sheet covers 4 × 4 logical units of the playfield, so a frame should be the hitbox divided by 4: 24 × 24 pixels for a box of 96 × 96. `EntityAppearances.EveryShippedSheetDrawsTheBoxTheServerHits` checks it for every shipped sheet. The side of the sheet, never its appearance, picks the layer the entity is drawn in (see [Client](/R-Type/client/#drawing-order-layer)). Whether the sprite sheet exists is checked when the game screen loads it, like any other asset.

### Attack animations

An enemy that attacks gives each phase of each attack an animation of its own, in a table `[appearance.attack_<n>.<phase>]`, `n` being the attack number (0 to 255) and `<phase>` one of `signal`, `strike`, `recover`, `enraged`, the phases of the `EnemyAction` message:

```toml
[appearance.attack_1.strike]
left = 0
top = 96
width = 48
height = 24
frames = 4
spacing = 2
frame_ms = 80
playback = "hold"
afterwards = "idle"
anchor_x = -12
anchor_y = 0
```

The frame keys are those of `idle`. The others:

| Key | Unit | Accepted | What it gives the animation |
|---|---|---|---|
| `playback` | | `loop`, `once`, `hold`; required | what follows the last frame: it starts again, or it stays on the last frame; `once` and `hold` both end once the last frame has shown |
| `afterwards` | | `idle`, `stay`; `idle` when left out | `idle` gives the body its idle animation back when the animation has ended; `stay` keeps the last frame |
| `anchor_x`, `anchor_y` | pixels of the canvas | -256 to 256; 0 when left out | how far the frames are drawn from the center of the body, so a frame wider than the body hangs from the side it grows on |

A phase the sheet leaves out has no animation: the enemy keeps what it plays when the server announces it. An attack table of another name, a phase of another name or any other key is refused. The end of an action is never announced, so the last phase of an attack plays `once` with `afterwards = "idle"` and lasts as long as the server keeps the enemy in that phase: the four 225 ms frames of the stunned Drillback last its 900 ms stun.

## When a program refuses a sheet

Each program stops on the first error and exits with code 1. A game process prints `game stopped: The module "games" failed to start:`, in its journal too, and its supervisor writes that the game crashed; the client prints `client stopped:`, then the absolute path of the file, the line and the column of what is wrong, the dotted name of the key and the rule it breaks. The messages below come from runs of the programs; the path is shortened to `…/entities/`.

| The sheet holds… | The program prints, after its prefix |
|---|---|
| a misspelt key (`top_sped`) | `…/entities/drifter.toml:21:1: unknown key gameplay.motion.top_sped` |
| a key less (`side`) | `…/entities/drifter.toml:11:1: gameplay.side is missing`, at the line of the table |
| a value out of its range (`points = 0` under `health`) | `…/entities/drifter.toml:27:10: gameplay.health.points must be an integer from 1 to 1000` |
| a number that is not an integer (`1500.0`) | `…/entities/drifter.toml:33:15: gameplay.gun.interval_ms must be an integer from 1 to 60000` |
| a pilot no behaviour has (`"zigzag"`) | `…/entities/drifter.toml:13:9: gameplay.pilot: unknown value "zigzag"; known: player, straight, weave` |
| a misspelt setting (`headin`) | `…/entities/drifter.toml:24:1: unknown key gameplay.behaviour.headin` |
| a weave too deep (`amplitude = 49`) | `…/entities/weaver.toml:24:13: gameplay.behaviour.amplitude must be an integer from 0 to 48` |
| a behaviour table on the ship | `…/entities/ship.toml:39:1: gameplay.behaviour: only a behaviour that declares settings has a behaviour table` |
| a gun on a missile | `…/entities/enemy_missile.toml:38:1: gameplay.gun: a missile has no gun` |
| a missile flying away from its edge (`heading = -1` on `player_missile`) | `…/entities/player_missile.toml:14:9: gameplay.pilot: a missile flies towards the edge that removes it: the right one for a missile of a player, the left one for a missile of an enemy` |
| a ship of another box | `…/entities/ship.toml:15:1: gameplay.hitbox: a ship collides with the box shipAfter() keeps on screen` |
| a projectile no sheet has (`"bullet"`) | `…/entities/drifter.toml:35:14: gameplay.gun.projectile: unknown value "bullet"; known: drifter, enemy_missile, player_missile, ship, weaver` |
| a projectile of the wrong side (`"player_missile"` on a Bydo) | `…/entities/drifter.toml:35:14: gameplay.gun.projectile: a player fires missiles of a player, an enemy missiles of an enemy` |
| no `[appearance]` | `…/entities/drifter.toml:1:1: appearance is missing`, from both programs |
| a misspelt key of the appearance (`frams`), on the client | `…/entities/drifter.toml:45:1: unknown key appearance.idle.frams` |
| a frame time of 0, on the client | `…/entities/drifter.toml:47:12: appearance.idle.frame_ms must be an integer from 1 to 60000` |
| a file named `Drifter.toml` | `The entity sheet …/entities/Drifter.toml must be named with 1 to 24 lowercase letters, digits or underscores` |
| no sheet named `ship` | `No entity sheet is named ship; the sheets are: drifter, enemy_missile, player_missile, weaver` |
| nothing: the folder holds no sheet | `The folder …/entities holds no entity sheet` |
| nothing: the folder is missing | `cannot read the file …/entities` |

## The shipped sheets

| Key | `drifter` | `weaver` | `ship` | `player_missile` | `enemy_missile` |
|---|---|---|---|---|---|
| `side` | `enemy` | `enemy` | `player` | `player_missile` | `enemy_missile` |
| `pilot` | `straight` | `weave` | `player` | `straight` | `straight` |
| `hitbox` | 96 × 96 | 96 × 96 | 128 × 64 | 64 × 24 | 32 × 32 |
| `motion` | `steady`, 240 | `steady`, 240 | `ship` | `steady`, 960 | `steady`, 360 |
| `behaviour` | `heading` -1 | `amplitude` 48, `period_ms` 2000 | none | `heading` 1 | `heading` -1 |
| `health` | 3 | 3 | none | none | none |
| `score` | 100 | 200 | none | none | none |
| `gun` | 1500 ms, first after 1500 ms, `enemy_missile` | 1500 ms, first after 1500 ms, `enemy_missile` | 333 ms, first at once, `player_missile` | none | none |
| `idle` on `sprites/entities.png` | 24 × 24, 2 frames of 250 ms | 24 × 24, 2 frames of 250 ms | 32 × 16, 2 frames of 80 ms | 16 × 6, 2 frames of 60 ms | 8 × 8, 2 frames of 120 ms |

The mobs and their projectiles draw from a sheet of their own in `sprites/mobs/`:

| Key | `voltjelly` | `reaper` | `drillback` | `bydomancer` | `rune_orb` | `orb_shard` |
|---|---|---|---|---|---|---|
| `side` | `enemy` | `enemy` | `enemy` | `enemy` | `enemy_missile` | `enemy_missile` |
| `pilot` | `straight` | `reaper` | `drillback` | `bydomancer` | `weave` | `scatter` |
| `hitbox` | 96 × 96 | 96 × 96 | 96 × 96 | 96 × 96 | 36 × 36 | 20 × 20 |
| `motion` | `steady`, 120 | `steady`, 300 | `steady`, 1120 | `steady`, 240 | `steady`, 360 | `steady`, 300 |
| `health` | 2 | 2 | 4, 1 while stunned (`fragile`) | 3 | none | none |
| `score` | 100 | 200 | 300 | 300 | none | none |
| `gun` | none | none | none | 1000 ms, first at once, `rune_orb` | none | none |
| contact | its aura hurts, 8 boxes; it survives | its scythe hurts, one box a frame | it survives | its nova hurts, two boxes a frame | | |
| `burst` | none | none | none | none | after 1500 ms, 4 `orb_shard` | none |
| `idle` | 40 × 40 (the bell 24 × 24), 4 frames of 120 ms | 24 × 24, 2 of 250 ms | 24 × 24, 2 of 200 ms | 24 × 24, 2 of 250 ms | 9 × 9, 2 of 120 ms | 5 × 5, 1 of 100 ms |

The values are **provisional** until gameplay tunes them; tuning them needs no rebuild. The Drifter and the Weaver share their frames, so the client draws both as the same Bydo. The settings of the behaviours are commented in each sheet.

## The shipped sheets and vanilla games

A changed or added sheet makes the games of the server not vanilla, and the journal says `not vanilla: the entity sheets are not the ones the game ships`. The server recognizes the shipped sheets by their fingerprint: SipHash-2-4 of the name and the bytes of each sheet, in the order of their names (`entitySheetsFingerprint()`, `src/server/Games/`), so a comment or a line ending counts as a change (see [Vanilla games](/R-Type/server/#vanilla-games-vanillabreaches)). `.gitattributes` keeps the sheets in LF on every checkout, Windows included. When a shipped sheet itself changes, `OFFICIAL_ENTITY_SHEETS_FINGERPRINT` in `src/server/ServerConstants.hpp` changes with it: `EntitySheetsFingerprint.TheShippedSheetsHaveTheOfficialFingerprint` fails until it does, and its failure prints the fingerprint the sheets now have.

## A client and a server with other sheets

A client draws a server with the sheets that server plays. When it accepts a player, the server announces its content pack: its sheets and the sprite sheets they name, in one file, named by its fingerprint (see [Protocol](/R-Type/protocol/#content-pack)). A client whose own `assets/` make the same pack plays with its own sheets. Any other client takes the pack from its cache, or downloads it from the supervisor while its connection screen shows `LOADING CONTENT` and a percentage, then draws the game from the sheets and sprite sheets of the pack; its own are never used in that session (see [Client](/R-Type/client/#entity-types-of-the-server-sessionentitytypes)). Of a sheet, the client reads the `side` of `[gameplay]` and the `[appearance]`; the rest of the gameplay plays no part on the client, since only the server runs it.

So a sheet added to the server, with its sprite sheet, reaches every player without a change to the clients, as long as the host opens the TCP port of the same number as the UDP port of the supervisor (see [Tutorial: publish a mod](/R-Type/tutorial-publish-a-mod/)). The path a sheet gives in `sheet` goes into the pack as it is written: lowercase letters, digits, `_`, `.`, `-` and `/`, 64 characters at most, or the supervisor stops when it starts.
