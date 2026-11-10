---
title: Scripting enemies in Luau
description: How a resource steers its enemies with Luau scripts the server runs, what a script reads and returns, and what the sandbox lets it do.
sidebar:
  label: Scripting in Luau
---

A resource (see [Resources](/R-Type/server/#resources-ensure-lines)) may steer its enemies with scripts written in [Luau](https://luau.org), the language derived from Lua 5.1 that Roblox made for untrusted code. A script registers a **behaviour**, the named piece of logic a sheet chooses with `pilot`, exactly like the behaviours written in C++ (`straight`, `reaper`, `bydomancer`): it reads the same view of its body each tick and returns the same intent. Changing a script needs no build: the next game reads it again.

Scripts run on the server only. They never reach the client, which draws the enemies from their sheets as before, so the authority of the server is unchanged. This page tells a designer what a script may read and return; the code is in `src/engine/Scripting/` (the machine, no R-Type word) and `src/game/Scripting/` (the behaviours).

## Where scripts live and when they run

Every `.luau` file right in the `server/` folder of a content folder is a server script: in each resource started by an `ensure` line, in the order the resources are laid, each folder in the order of its file names. The supervisor runs each script once when it starts, before it reads the sheets to list the difficulties of the waves files, and each game process runs them again when its games module starts; so a sheet may name a behaviour a script registers. A script that does not compile, fails while it runs, or calls `register` wrongly stops the supervisor at start, before any game, and its error names the script and the line.

```
resources/crimson-drifter/
├── resource.toml
├── entities/crimson_drifter.toml   pilot = "crimson_charge", and its settings
└── server/charge.luau              registers crimson_charge
```

## Registering a behaviour

```lua
behaviours.register("crimson_charge", {
	{ name = "align_range", lowest = 0, highest = 1080 },
	{ name = "cruise_speed", lowest = 0, highest = 1920 },
	{ name = "charge_speed", lowest = 0, highest = 1920 },
}, function(view)
	return { direction = { x = -1, y = 0 }, top_speed = view.settings.cruise_speed }
end)
```

`behaviours.register(name, settings, steer)` takes the name a sheet gives `pilot`, the settings the sheet must give in `[gameplay.behaviour]`, each a whole number from `lowest` to `highest` (a sheet that misses one, misspells one or gives a value out of its range is refused, as for a C++ behaviour), and the function called each tick for each body that names the behaviour. A name a behaviour already has, a built-in one included, stops the supervisor at start. The function keeps no state between ticks, as every behaviour: one function pilots every body of that type, and it decides from the view alone.

## What the function reads: the view

| Key | Holds |
|---|---|
| `position` | where the center of the body is, `{x, y}` in logical units |
| `origin` | where it appeared |
| `age_ms` | how long it has existed, in milliseconds |
| `elapsed_ms` | how long this tick lasts |
| `top_speed` | its speed at a full direction, in logical units per second |
| `settings` | the values its sheet gives, by name |
| `action` | the attack and the phase it is in, `{attack, phase}`, the phase `"signal"`, `"strike"`, `"recover"` or `"enraged"`; absent while it is idle |
| `action_age_ms` | how long it has been in that action, or idle |
| `nearest_ship` | the center of the nearest ship of a player, `{x, y}`; absent when no ship is in play |
| `wounded` | whether it lost health points |
| `health` | the share of its health points left, 0 to 1 |
| `enraged` | whether it is a boss that enraged, as the keys `boss` of its [sheet](/R-Type/entity-sheets/) set; false for any other body |
| `actions_ended` | how many actions it has ended since it appeared |

## What the function returns: the intent

The function returns a table; every key is optional, and a key left out, or of the wrong type, asks for nothing.

| Key | Asks |
|---|---|
| `direction` | where to go, `{x, y}`, each a share of its top speed from -1 to 1; a larger value is cut to 1 |
| `shooting` | `true` to fire its gun, as its sheet sets it |
| `starts_action` | to enter an attack and a phase, `{attack, phase}`: the client plays the animation the sheet gives that phase, and its hurt boxes apply |
| `ends_action` | `true` to leave its action |
| `jumps_to` | to appear at `{x, y}` without travelling there |
| `collision_box` | to take another size of collision box, `{width, height}` in logical units, both positive; the offset of the box from the center, which a boss sheet may give, stays |
| `top_speed` | to take another top speed, positive, in logical units per second |
| `effect` | to play once the effect its sheet names so in `[appearance.effects]`; the server announces it to every player, guaranteed |
| `look` | to show from now on the look its sheet names so in `[appearance.looks]`, or `""` for its own idle loop; every world state carries it, so a player who joins later sees it too |

The collision box, the top speed and the look stay until a later tick asks for others; an effect plays at each tick that names it. An `effect` or a `look` must be a name of the content, 1 to 24 bytes of `a` to `z`, digits and `_`, otherwise it is left out; a name the sheet of the body does not hold is left out too, silently (see [Named effects and looks](/R-Type/entity-sheets/#named-effects-and-looks)). The script of the example asks for its speed every tick, so its Drifter charges only while a ship is level with it.

## What a script cannot do

- **Reach the system.** The sandbox of Luau removes `io`, `package`, `require`, `dofile`, `loadfile` and `load`, leaves `os` its clock and dates only, and makes every standard library read-only; R-Type also removes `getfenv` and `setfenv`. Each script has globals of its own: a global of one script is not seen by another.
- **Hold the server.** All the scripts of a game share 16 MiB (`SCRIPT_MEMORY_LIMIT_BYTES`), and each run of a script and each call of a function has 20 ms (`SCRIPT_CALL_BUDGET`): past it, the script is stopped at its next loop or call. A function that fails, runs past its budget or asks for too much memory leaves its body still for that tick, and the journal of the game writes the first failure of each behaviour once.
- **Create or remove entities.** A script steers its own body; creating a sheet next to it, a brood or a volley, is not written yet. A boss does it through its sheet: its attacks and their phases.
- **Draw something its sheet does not.** A script names an effect or a look its sheet draws; a new animation, a sound, or an effect outside the sprite sheet of the type belongs to the sheet and the client, not to a script.

## Checking a script before a game

`luau-analyze`, the analyzer of Luau, reports syntax errors and, with `--!strict` at the top of a file, the values a script uses wrongly; the example script starts with it. The project does not ship a definition of `behaviours` for it yet, so the analyzer may report `behaviours` as an unknown global: that line is expected. This check was not run for this page.

## What a script costs

`script_budget_bench` (`tests/tools/ScriptBudgetBench/`) measures what the scripts take of a tick, so that a level is written knowing how many scripted bodies the server holds. It keeps alive 300 bodies steered by the script of the Crimson Drifter (`--scripted <count>`), 1000 bullets flown by the built-in behaviour `straight` (`--bullets <count>`) and four ships, and computes 3600 ticks of 16.7 ms (`--ticks <count>`), one after the other with no wait. Each tick runs `PilotSystem`, `MovementSystem` and `PlayfieldExitSystem`, then replaces the bodies that left the playfield, so the load stays the same. Nothing is random, so two runs do the same work.

```bash
cmake --preset bench
cmake --build --preset bench --target script_budget_bench
./build-bench/tests/script_budget_bench
```

The `bench` preset is the `test` preset in Release: a build that is not optimized says so on its error output, since its figures say nothing of a server. The report gives the median, the 99th percentile and the worst tick, the same for the scripts alone, the share of the scripts in the time of all the ticks, and how many ticks passed 16.7 ms. The time of the scripts is read around each call, garbage collection included; the two clock reads of a call count in it (14 ns each, about 3 % of a call).

Measured on an Apple M4 Pro with Apple clang 21 over 3600 ticks: three runs for the first row, two for the second, one for the third.

| Scripted bodies, bullets | Median tick | Worst tick | Median of the scripts | Share of the scripts |
|---|---|---|---|---|
| 300, 1000 | 0.74 to 0.75 ms | 1.3 to 4.5 ms | 0.27 ms | 36 % |
| 3000, 1000 | 4.3 to 4.4 ms | 6.0 to 14.2 ms | 2.7 to 2.8 ms | 63 % |
| 6000, 1000 | 8.3 ms | 14.0 ms | 5.5 ms | 67 % |

A call of the script takes 0.9 µs, and the time grows in line with the bodies from 300 to 6000. The medians repeat within 2 %; the worst tick varies from run to run, since it depends on what else the machine does. These figures hold for this script, which reads a few keys and returns one table, and for a tick without collisions, weapons or the sending of the world state: they are the cost of the scripts, not of a whole game tick.

## Where to intervene

| You want to… | Look at |
|---|---|
| Change what a script reads or may return | `pushBehaviourView()` and `intentAt()` in `src/game/Scripting/ScriptView.cpp`, the keys in `GameScriptingConstants.hpp` |
| Change the memory or the time a script has | `SCRIPT_MEMORY_LIMIT_BYTES` and `SCRIPT_CALL_BUDGET` in `src/engine/Scripting/ScriptingConstants.hpp` |
| Expose another function to the scripts | `ScriptMachine::expose()`, before the first script runs, as `BehaviourScripts` exposes `behaviours.register` |
| Change which files are scripts | `scriptSourcesIn()` in `src/game/Scripting/BehaviourScriptFiles.cpp` |
