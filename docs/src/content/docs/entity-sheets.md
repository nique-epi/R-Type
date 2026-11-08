---
title: Writing an entity sheet
description: How to describe a type of entity in assets/entities/, the keys of its gameplay and of its appearance, the rules between them, and the error a program prints when a sheet is wrong.
sidebar:
  label: Entity sheets
---

An entity sheet, `assets/entities/<name>.toml`, describes one type of entity: the ship of a player, a Bydo, a boss, a missile. This page is for whoever adds or tunes a type: it gives the keys, what each one gives an entity, the rules between keys and between sheets, and how to read the error a program prints, without opening the C++. Why the types are data and not code is told in [Architecture](/R-Type/architecture/#entity-types).

Both programs read the folder `entities` (`ENTITY_SHEET_FOLDER_IN_ASSETS`) of the `assets/` folder next to their executable, or one level above it, once, when they start; the folder they are started from plays no part. On the server, each game process reads the table `[gameplay]` of each sheet (`EntitySheets`, `src/game/EntityTypes/`) when its games module starts, so when the first player who needs that game connects; the supervisor reads no sheet, before the waves file, which names its enemies among the sheets. The client reads the table `[appearance]` (`EntityAppearances`, `src/client/Rendering/Sprites/EntityAppearances/`) when its assets module is installed, before it opens its window. Each checks that the root of a sheet holds exactly these two tables, and only the keys of its own: the server never reads `[appearance]`, the client never reads `[gameplay]`.

## The folder and the names

- Every `.toml` file of the folder is a sheet; any other file, and every sub-folder, is left alone. The name of the file without `.toml` is the name of the type: `weaver.toml` is `weaver`.
- A name is 1 to 24 bytes (`MAXIMUM_ENTITY_TYPE_NAME_LENGTH`) of lowercase letters `a` to `z`, digits and `_`, so a file has the same name on every system. A folder holds at most 32 sheets (`MAXIMUM_ENTITY_TYPES`) and at least one. Both limits and the check of a name (`isEntityTypeName()`) live in `src/identifiers/`, so a name the game accepts is one the table of the types can carry.
- The server numbers the sheets from 1 in the order of their names. The twenty-three sheets the game ships are `bydo_tyrant` (1), `bydomancer` (2), `drifter` (3), `drillback` (4), `enemy_missile` (5), the seven look items `look_chef_toque` (6) to `look_wizard_hat` (12) in the order of their names, `orb_shard` (13), `player_missile` (14), `reaper` (15), `rune_orb` (16), `ship` (17), `tyrant_diver` (18), `tyrant_missile` (19), `tyrant_spore` (20), `tyrant_spore_shard` (21), `voltjelly` (22) and `weaver` (23). These numbers name the types on the wire for as long as the server runs, and every player learns them when it is accepted (see [Protocol](/R-Type/protocol/#entity-types)), so a new sheet whose name sorts first renumbers the others without breaking anything.
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

`World::spawn(sheet, position, owner)` (`src/game/World.hpp`) makes every entity of the game from its sheet, with the components below, an `Intent` and a `NetworkIdentity`. A body no behaviour pilots is the ship of a player: it also gets `STARTING_LIVES` (3) lives, which the server replaces with the lives of the rules of its game, and, with `owner`, a `ShipPilot` naming that player, so its own missiles never hit it. Any other body given `owner` gets a `MissileOwner`, so its kills are credited to that player.

| Key | Unit | Accepted | What it gives the entity |
|---|---|---|---|
| `side` | | `player`, `enemy`, `player_missile`, `enemy_missile`, `look_item` | its `EntityType`: what it hits and what hits it (see [Collisions](/R-Type/server/#rules-collisionrules)), and the layer the client draws it in (a `look_item` is drawn in the layer of the Bydos) |
| `pilot` | | `player`, or the name of a behaviour: `straight`, `weave`, `scatter`, `reaper`, `drillback`, `bydomancer`, `bydo_tyrant`, or one a server script of a resource registers | what writes its `Intent`: the input of its player, or that behaviour (see [Behaviours](#behaviours-and-their-settings) and [Scripting in Luau](/R-Type/scripting/)) |
| `hitbox.width`, `hitbox.height` | logical units | 1 to 1024 | its `CollisionBox`, centered on its position |
| `hitbox.offset_x`, `hitbox.offset_y` | logical units | -1024 to 1024; 0 when left out | where the center of its `CollisionBox` lies from its position, y growing downwards: the box of the Bydo Tyrant covers its body, 12 units right of and 4 below its center |
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
| `hurt_zone.beam` | | an array of tables, `[[gameplay.hurt_zone.beam]]`; may be left out | beams that hurt a ship that touches them, besides the boxes |
| `hurt_zone.beam.offset_x`, `offset_y` | logical units | -1024 to 1024 | the pivot of the beam, from the position of the body |
| `hurt_zone.beam.length`, `thickness` | logical units | `length` 1 to 4096, `thickness` 1 to 1024 | a band `thickness` thick, `length` long from its pivot |
| `hurt_zone.beam.from_deg`, `to_deg` | degrees | -180 to 180 | its angle from the left, a positive angle turning it down: it goes evenly from `from_deg` to `to_deg` over its window, so a sweep is two angles and a window |
| `hurt_zone.beam.attack`, `phase` | | as for a box, both required | the attack and the phase the beam hurts in |
| `hurt_zone.beam.from_ms`, `until_ms` | milliseconds | 0 to 60000 | its window within its phase, as for a box |
| `[gameplay.survives_contact]` | | an empty table; enemies only | the `SurvivesContact` marker: a ship that touches the body loses a life, and the body stays |
| `fragile.attack`, `fragile.phase` | | as for a box, both required; enemies only | its `Fragile`: any hit while it is in that phase of that attack takes every point it has left, as the Drillback stunned |
| `burst.after_ms` | milliseconds | 1 to 60000; missiles only | its `ShardBurst`: how long it lives before it bursts |
| `burst.count` | shards | 1 to 8 | how many shards it bursts into, spread evenly around it from the diagonal up and to the right: 4 fly along the four diagonals |
| `burst.shard` | | the name of a sheet of the same side | what it bursts into |
| `volley` | | an array of tables, `[[gameplay.volley]]`; enemies only | bodies that appear at once as the enemy enters an action: a fan of missiles, a column of Bydos, a wall with a gap; an enemy among them stays harmless for 1 s first (see [Volleys](/R-Type/server/#volleys-volleysystem)) |
| `volley.attack`, `volley.phase` | | as for a box, both required | the action that fires the volley, on the tick the enemy enters it |
| `volley.body` | | the name of a sheet of side `enemy` or `enemy_missile` | what the volley is made of |
| `volley.count` | bodies | 1 to 16 | how many bodies it holds, numbered from 0 |
| `volley.offset_x`, `offset_y` | logical units | -1920 to 1920 | where body 0 starts from the position of the enemy, so a volley may be laid anywhere on the playfield |
| `volley.step_x`, `step_y` | logical units | -1920 to 1920; 0 when left out | how far each body starts from the one before it |
| `volley.heading_deg` | degrees | -80 to 80; 0 when left out | the heading of a volley of one body, and the middle of the fan of a spread: left turned by this angle, a positive angle turning down |
| `volley.spread_deg` | degrees | 0 to 360; 0 when left out | the fan of the headings: from the heading turned by -`spread_deg` / 2 for body 0 to +`spread_deg` / 2 for the last, evenly |
| `volley.distance` | logical units | 0 to 1024; 0 when left out | how far along its heading from its start a body appears; it steers from its start, so `scatter` flies it straight away from there |
| `volley.gap_from`, `gap_count` | bodies | 0 to 15; 0 when left out | the bodies left out, from body `gap_from`, `gap_count` of them: the gap of a wall |
| `volley.jitter_x`, `jitter_y` | logical units | 0 to 960; 0 when left out | how far the whole volley may move at random each time it fires, on each axis |
| `volley.chance_percent` | percent | 1 to 100; 100 when left out | the chance the volley fires when its action starts, drawn each time, so two volleys of one action fire or not apart |
| `boss.rage_below_percent` | percent | 1 to 99 | its `Boss`: it enrages once its health falls below its full health times this share, rounded down |
| `boss.rage_attack` | | 0 to 255 | the attack it starts, in the phase `enraged`, when it enrages |
| `boss.arrival_attack` | | 0 to 255 | the attack of its arrival: while it is in any phase of it, a hit takes nothing from it |
| `look_item` | | the name of an item of the locker: `top_hat`, `party_hat`, `crown`, `propeller_cap`, `cowboy_hat`, `viking_helmet`, `wizard_hat`, `chef_toque`, `cool_shades`, `mustache`; for side `look_item` only | its `LookItem`: the item the pilot whose ship touches the body wins for the locker of the hangar (see [Ship looks](/R-Type/server/#look-items-lookitemkeeper)) |

- `side`, `pilot`, `hitbox` and `motion` are required; the tables `behaviour`, `health`, `score`, `gun`, `hurt_zone`, `survives_contact`, `fragile`, `burst`, `volley` and `boss` may be left out. The three keys of `boss` are required in it. A body without `health` takes no damage: a missile that hits it still disappears, and a ship pays a hit with a life instead (see [Lives](/R-Type/server/#lives-lifekeeper)). A body without `score` earns nothing, without `gun` fires nothing.
- Any other key is refused, so a misspelt key stops the program instead of being ignored. Every number is a TOML integer: `240`, never `240.0` or `"240"`.
- A duration of the gun becomes the nearest whole number of ticks of a sixtieth of a second: 333 ms are 20 ticks, 1500 ms are 90. The interval is at least 1 tick (`MINIMUM_GUN_INTERVAL_TICKS`), and so is the time before a burst (`MINIMUM_BURST_TICKS`).
- The keys, the choices and the ranges live in `src/game/EntityTypes/EntitySheetConstants.hpp`.

## Rules between keys and between sheets

A sheet whose keys are each valid is refused when it does not hold together:

- A `player` pilot steers a body of side `player`, and only such a body.
- A body of side `player` moves as a `ship`, and only such a body. Its hitbox is 128 × 64 (`SHIP_WIDTH` × `SHIP_HEIGHT`, `src/game/ShipControl/ShipControlConstants.hpp`), centered on its position (no `offset_x` or `offset_y` other than 0), the box `shipAfter()` keeps on screen.
- A missile, of side `player_missile` or `enemy_missile`, has no gun.
- A missile flies towards the edge that removes it (see [Piloted bodies](/R-Type/server/#piloted-bodies-pilotsystem-and-playfieldexitsystem)): on its first tick its pilot steers a missile of side `player_missile` to the right and one of side `enemy_missile` to the left. A `straight` heading of 1 for the first, of -1 for the second; the `weave` goes left, and so does `scatter` on its first tick, still on the point it steers from, so both may pilot a missile of an enemy only. A missile flying the other way, or staying, would never be removed when it hits nothing. The check looks at the first tick only: a missile that turns later, as a shard of a burst, leaves by any edge it flies towards.
- `[gameplay.behaviour]` stands only when the behaviour of the pilot declares settings, and then holds exactly those settings.
- A gun fires a sheet of the folder: of side `player_missile` for a shooter of side `player`, of side `enemy_missile` for any other shooter.
- Only a body of side `enemy` has a `hurt_zone`, `survives_contact`, `fragile`, `volley` or `boss`, and a `hurt_zone` holds at least one box.
- A `boss` has a `health`: its bar shows it and its rage reads it.
- A volley is made of a sheet of the folder of side `enemy` or `enemy_missile`, and its gap leaves at least one body: `gap_count` is below `count`.
- A beam that sweeps, `to_deg` other than `from_deg`, names `until_ms`, the end of its sweep; a beam without `until_ms` keeps `from_deg`.
- Only a missile has a `burst`, and it bursts into missiles of its own side, of a sheet of the folder.
- A body of side `look_item` names its item with `look_item`, and no other side does. On its first tick the pilot of a look item steers it to the left, the edge that removes it, so an item nobody picks up does not stay for the rest of the game. Its hitbox is the size it is drawn at, as for every shipped sheet.
- A box names its `attack` and its `phase` together, or neither; a box that names neither has no `from_ms` or `until_ms`, and `until_ms` is above `from_ms`. An `attack` is a number the sheet gives its own attacks: the code knows no attack by name.
- The waves file names enemies among the sheets of side `enemy`, and the boss of a difficulty among the sheets that have a `boss` (see [Writing the waves file](/R-Type/waves-file/#enemies)).

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
| `bydo_tyrant` | arrives first (attack 0): slides on its row from where it appears to `rest_x` over `slide_ms` (signal), then roars `roar_ms` (strike). Then, rested `rest_ms`, it lashes its tentacles (attack 4, 14 enraged) at a ship within `melee_reach` of its melee point (`melee_x`, `melee_y` from its center), or takes the next attack of its rotation, picked by the count of the actions it ended: brood 3, burst 1, beam 2, burst 1 while calm; charge 18, burst 11, sweep 15, spores 16, beam 12, wall 17, brood 13 once enraged. Between two attacks it drifts up or down, at 240 units per second at most and within 200 units of the row it arrived on, towards the row of the nearest ship and a row drawn from its count of actions in turn; before a brood, its spores or its wall, which are laid out from that row, it heads back to it. It holds still `rage_ms` as it enrages, and its charge crosses the playfield to the left, then, once its whole body left the screen, comes back on the same row from past the right edge. An enraged variant signals `rage_signal_percent` of the calm one | `rest_x`, `rest_ms`, `slide_ms`, `roar_ms`, `settle_ms`, `<attack>_signal_ms`, `<attack>_strike_ms` and `<attack>_recover_ms` for `burst`, `beam`, `brood`, `tentacle`, `sweep`, `spores`, `wall` and `charge`, `melee_x`, `melee_y`, `melee_reach`, `rage_ms`, `rage_signal_percent` |

`straight`, `weave` and `scatter` want to fire at every tick, so a body fires as often as its gun allows; a body without a gun fires nothing, and no missile has one, so the diver of the Tyrant scatters and fires while a shard never fires. `reaper`, `drillback` and `bydo_tyrant` never fire: the missiles and Bydos of the Tyrant are the volleys of its sheet; `bydomancer` wants to fire during the strike of its orb only, so its gun, ready at once and waiting longer than a strike, fires one orb a cast. The settings of an attack are milliseconds (`rest_ms` 0 to 60000, `signal_ms` and `strike_ms` 1 to 60000, `recover_ms` 0 to 60000, 0 skipping the recover); the others are logical units. A behaviour that attacks steps through its phases with `ActionCycle` and starts an attack again once it has been idle `rest_ms` (see [Piloted bodies](/R-Type/server/#piloted-bodies-pilotsystem-and-playfieldexitsystem)). A weave needs a top speed of at least 2π × `amplitude` ÷ `period_ms`, 151 logical units per second for the Weaver: a slower body falls behind its weave. A new behaviour is a class implementing `IBehaviour`, added to `builtInBehaviours()`, as the [tutorial](/R-Type/tutorial-add-an-enemy-type/) shows.

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
| `[appearance.enraged_idle]` | | the keys of `idle`; may be left out | the idle animation of an enemy once the server announced it enraged, which every attack that ends with `afterwards = "idle"` goes back to; the enemy keeps `idle` without it |
| `[appearance.death]` | | the keys of `idle`; may be left out | the death the client plays once, centered where the enemy died, when the server says it was killed; its version without flash, played with REDUCE FLASHES, is the row right under it, frame for frame, `height` + `spacing` pixels lower. An enemy without it plays the Bydo explosion of `sprites/effects.png` |

Every key of `idle` is required, `enraged_idle` and `death` are the only optional tables besides the attack animations, and any other key is refused. The keys and ranges live in `src/client/Rendering/Sprites/EntityAppearanceConstants.hpp`. A pixel of a sheet covers 4 × 4 logical units of the playfield, so a frame should be the hitbox divided by 4: 24 × 24 pixels for a box of 96 × 96. `EntityAppearances.EveryShippedSheetDrawsTheBoxTheServerHits` checks it for every shipped sheet whose boxes all hurt in an action only. A sheet with a box that always hurts, the Voltjelly and the Bydo Tyrant, draws a frame that holds its hitbox, offset included, and every such box instead: the Tyrant is drawn 128 × 128 pixels, 512 × 512 units, and hit on a box of 440 × 488 that covers its body (`AFrameHoldsTheBoxesThatAlwaysHurt`). The side of the sheet, never its appearance, picks the layer the entity is drawn in (see [Client](/R-Type/client/#drawing-order-layer)). Whether the sprite sheet exists is checked when the game screen loads it, like any other asset.

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
| `death` | 48 × 48, 6 of 70 ms | 48 × 48, 6 of 70 ms | 48 × 48, 6 of 70 ms | 48 × 48, 6 of 70 ms | none | none |

The final boss, the diver it calls in and its projectiles: the Tyrant draws from `sprites/mobs/bydo_tyrant.png`, its spore too, its diver from the idle frames of `sprites/mobs/drillback.png`, and its missiles and the shards of a spore from `sprites/entities.png`, as the missile of a Bydo.

| Key | `bydo_tyrant` | `tyrant_diver` | `tyrant_missile` | `tyrant_spore` | `tyrant_spore_shard` |
|---|---|---|---|---|---|
| `side` | `enemy` | `enemy` | `enemy_missile` | `enemy_missile` | `enemy_missile` |
| `pilot` | `bydo_tyrant` | `scatter` | `scatter` | `straight`, `heading` -1 | `scatter` |
| `hitbox` | 440 × 488, 12 units right of and 4 below its center: its body | 96 × 96 | 32 × 32 | 36 × 36 | 32 × 32 |
| `motion` | `steady`, 2500, fast enough for its charge | `steady`, 360 | `steady`, 420 | `steady`, 1: a mine | `steady`, 300 |
| `health` | 240, scaled when it appears (see [Boss](/R-Type/server/#boss-bossstage-and-bossphasesystem)) | 2 | none | none | none |
| `score` | 10000 | 150 | none | none | none |
| `gun` | none | 1800 ms, first after 800 ms, `enemy_missile` | none | none | none |
| `boss` | enrages below 50 %, rage attack 9, arrival attack 0 | | | | |
| contact | it survives; its body hurts, two boxes, its tentacles a box in attacks 4 and 14, its beam in 2, 12 and 15, the last sweeping from -35 to 12 degrees over 1200 ms | | | | |
| `volley` | 5 `tyrant_missile` over 48 degrees in attack 1, 7 in attack 11; a brood in front of it in attacks 3 and 13, one volley of one body each: a `weaver` always, then two `tyrant_diver` from the top and the bottom edges at 28 degrees, a `voltjelly`, a `reaper` and a `bydomancer`, each by its chance, 70, 70, 60, 40 and 30 % (90, 90, 80, 60 and 50 % enraged), and each moved at random by its jitter; 5 `tyrant_spore` on a diagonal in attack 16; a wall of 9 `drifter` with 2 left out in attack 17 | | | | |
| `burst` | | none | none | after 1100 ms, 4 `tyrant_spore_shard` | none |
| `idle` | 128 × 128, 2 frames of 300 ms; enraged, 2 of 200 ms | 24 × 24, the idle frames of the Drillback, 2 of 120 ms | 8 × 8, 2 of 120 ms | 9 × 9, 2 of 200 ms | 8 × 8, 2 of 120 ms |

The values are **provisional** until gameplay tunes them; tuning them needs no rebuild. The Drifter and the Weaver share their frames, so the client draws both as the same Bydo; the diver of the Tyrant shares the idle frames of the Drillback, at a frame time of its own. The settings of the behaviours are commented in each sheet.

The seven look items are bodies of side `look_item` that float to the left, `steady` at 240 units per second, piloted by `straight` with `heading` -1, with no health, score or gun. They draw from `sprites/looks.png`, one frame each:

| Sheet | `look_item` | Hitbox | Frame on the sheet |
|---|---|---|---|
| `look_party_hat` | `party_hat` | 48 × 44 | 12 × 11 at 18 |
| `look_crown` | `crown` | 48 × 32 | 12 × 8 at 36 |
| `look_propeller_cap` | `propeller_cap` | 48 × 32 | 12 × 8 at 54 |
| `look_cowboy_hat` | `cowboy_hat` | 56 × 32 | 14 × 8 at 72 |
| `look_viking_helmet` | `viking_helmet` | 56 × 36 | 14 × 9 at 90 |
| `look_wizard_hat` | `wizard_hat` | 48 × 48 | 12 × 12 at 108 |
| `look_chef_toque` | `chef_toque` | 48 × 36 | 12 × 9 at 126 |

The top hat, the cool shades and the mustache are in the locker from the first launch and have no sheet.

## The shipped sheets and vanilla games

A changed or added sheet makes the games of the server not vanilla, and the journal says `not vanilla: the entity sheets are not the ones the game ships`. The server recognizes the shipped sheets by their fingerprint: SipHash-2-4 of the name and the bytes of each sheet, in the order of their names (`entitySheetsFingerprint()`, `src/server/Games/`), so a comment or a line ending counts as a change (see [Vanilla games](/R-Type/server/#vanilla-games-vanillabreaches)). `.gitattributes` keeps the sheets in LF on every checkout, Windows included. When a shipped sheet itself changes, `OFFICIAL_ENTITY_SHEETS_FINGERPRINT` in `src/server/ServerConstants.hpp` changes with it: `EntitySheetsFingerprint.TheShippedSheetsHaveTheOfficialFingerprint` fails until it does, and its failure prints the fingerprint the sheets now have.

## A client and a server with other sheets

A client draws a server with the sheets that server plays. When it accepts a player, the server announces its content pack: its sheets and the sprite sheets they name, in one file, named by its fingerprint (see [Protocol](/R-Type/protocol/#content-pack)). A client whose own `assets/` make the same pack plays with its own sheets. Any other client takes the pack from its cache, or downloads it from the supervisor while its connection screen shows `LOADING CONTENT` and a percentage, then draws the game from the sheets and sprite sheets of the pack; its own are never used in that session (see [Client](/R-Type/client/#entity-types-of-the-server-sessionentitytypes)). Of a sheet, the client reads the `side` of `[gameplay]` and the `[appearance]`; the rest of the gameplay plays no part on the client, since only the server runs it.

So a sheet added to the server, with its sprite sheet, reaches every player without a change to the clients, as long as the host opens the TCP port of the same number as the UDP port of the supervisor (see [Tutorial: publish a mod](/R-Type/tutorial-publish-a-mod/)). The path a sheet gives in `sheet` goes into the pack as it is written: lowercase letters, digits, `_`, `.`, `-` and `/`, 64 characters at most, or the supervisor stops when it starts.
