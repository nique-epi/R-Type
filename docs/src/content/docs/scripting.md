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

`behaviours.register(name, settings, steer)` takes the name a sheet gives `pilot`, the settings the sheet must give in `[gameplay.behaviour]`, each a whole number from `lowest` to `highest` (a sheet that misses one, misspells one or gives a value out of its range is refused, as for a C++ behaviour), and the function that steers each body that names the behaviour. A name a behaviour already has, a built-in one included, stops the supervisor at start. Each body runs that function in a thread of its own (a Luau coroutine), which ends with the body whichever way it is destroyed: a function that returns gives the intent of that tick and starts again from its first line at the next tick, and a function that waits keeps its local variables until the wait ends (see [Waiting, creating and removing bodies](#waiting-creating-and-removing-bodies)).

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

The collision box and the top speed stay until a later tick asks for others. The script of the example asks for its speed every tick, so its Drifter charges only while a ship is level with it.

## Waiting, creating and removing bodies

```lua
behaviours.register("sentry", {}, function(view)
	behaviours.wait(500, { direction = { x = -1, y = 0 } })
	local drone = behaviours.spawn("crimson_drifter", { x = view.position.x, y = view.position.y })
	view = behaviours.wait(2000, { direction = { x = 0, y = 0 }, shooting = true })
	behaviours.destroy(drone)
	return { direction = { x = -1, y = 0 } }
end)
```

| Function | Does |
|---|---|
| `behaviours.wait(ms, intent)` | stops the function there: the body wants `intent` on every tick of the wait, its one-time requests (`starts_action`, `ends_action`, `jumps_to`) on the first tick only, and the call returns a fresh view on the tick the wait ends. The wait lasts `ms` milliseconds rounded to the nearest tick (16.7 ms), one tick at least; a waiting body costs no call. It only runs in the function of a body; `coroutine.yield` called directly is a failure. |
| `behaviours.spawn(type, {x, y})` | asks for a body of the sheet `type` at `{x, y}`, and returns its network id at once. It appears at the end of the tick, after every body was steered, and every player is told. A sheet that does not exist or that ships are made from is refused, and so are more than 256 bodies in one tick (`MAXIMUM_SCRIPTED_SPAWNS_PER_TICK`). |
| `behaviours.destroy(id)` | removes the body of that network id at the end of the tick, after the bodies created in it; an id no body has does nothing, a ship is never removed, and more than 256 removals in one tick are refused (`MAXIMUM_SCRIPTED_REMOVALS_PER_TICK`). |
| `view.id` | the network id of the body being steered, the one `spawn` returned for it. |

Both `spawn` and `destroy` only run during a game: a script that calls them while it is first read fails.

## What a script cannot do

- **Reach the system.** The sandbox of Luau removes `io`, `package`, `require`, `dofile`, `loadfile` and `load`, leaves `os` its clock and dates only, and makes every standard library read-only; R-Type also removes `getfenv` and `setfenv`. Each script has globals of its own: a global of one script is not seen by another.
- **Hold the server.** All the scripts of a game share 16 MiB (`SCRIPT_MEMORY_LIMIT_BYTES`), and the machine collects every garbage at the start of a tick once they hold 75 % of it. Reading a script has 20 ms (`SCRIPT_CALL_BUDGET`). Each step of a body, from one wait to the next, has 2 ms (`SCRIPT_STEP_BUDGET`), and the steps of one tick have 8 ms together (`SCRIPT_TICK_BUDGET`): once those are spent, the bodies still due keep what they wanted and step at the next tick, and the journal of the game says so once. A step is stopped only after 1024 loops or calls past its deadline (`SCRIPT_OVERDUE_SAFEPOINTS`), so a pause of the machine does not stop a body that did nothing wrong.
- **Survive a failure.** A step that fails, runs past its budget or asks for too much memory stops the script of that body for good: the body is removed from the game at the end of the tick, so a body frozen out of reach cannot keep a level from ending; the other bodies go on, and the journal writes the first failure of each behaviour once.
- **Change what the client draws.** A new animation, sound or effect belongs to the sheet and the client, not to a script.

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

`--waiting` runs the same bodies with a script that waits one tick at a time and never returns, so each tick resumes every coroutine: the most a waiting body can cost.

Measured on an Apple M4 Pro with Apple clang 21 over 3600 ticks: three runs for the first row, two for the second, one for the third, three for the last.

| Scripted bodies, bullets | Median tick | Worst tick | Median of the scripts | Share of the scripts |
|---|---|---|---|---|
| 300, 1000 | 0.74 to 0.75 ms | 1.3 to 4.5 ms | 0.27 ms | 36 % |
| 3000, 1000 | 4.3 to 4.4 ms | 6.0 to 14.2 ms | 2.7 to 2.8 ms | 63 % |
| 6000, 1000 | 8.3 ms | 14.0 ms | 5.5 ms | 67 % |
| 300 waiting, 1000 | 0.77 to 0.78 ms | 1.7 to 4.9 ms | 0.27 ms | 36 % |

A call of the script takes 0.9 µs, and the time grows in line with the bodies from 300 to 6000. The medians repeat within 2 %; the worst tick varies from run to run, since it depends on what else the machine does. These figures hold for this script, which reads a few keys and returns one table, and for a tick without collisions, weapons or the sending of the world state: they are the cost of the scripts, not of a whole game tick.

## Where to intervene

| You want to… | Look at |
|---|---|
| Change what a script reads or may return | `pushBehaviourView()` and `intentAt()` in `src/game/Scripting/ScriptView.cpp`, the keys in `GameScriptingConstants.hpp` |
| Change the memory or the time a script has | `SCRIPT_MEMORY_LIMIT_BYTES`, `SCRIPT_CALL_BUDGET`, `SCRIPT_STEP_BUDGET` and `SCRIPT_TICK_BUDGET` in `src/engine/Scripting/ScriptingConstants.hpp` |
| Change how a body waits or what it remembers | `ScriptedBehaviour::steer()` and `ScriptedRoutine` in `src/game/Scripting/` |
| Change how created or removed bodies reach the game | `ScriptedBodies` in `src/server/Spawning/` |
| Expose another function to the scripts | `ScriptMachine::expose()`, before the first script runs, as `BehaviourScripts` exposes `behaviours.register` |
| Change which files are scripts | `scriptSourcesIn()` in `src/game/Scripting/BehaviourScriptFiles.cpp` |
