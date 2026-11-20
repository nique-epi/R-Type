---
title: Scripting enemies in Luau
description: How a resource steers its enemies with Luau scripts the server runs, what a script reads and returns, and what the sandbox lets it do.
sidebar:
  label: Scripting in Luau
---

A resource (see [Resources](/R-Type/server/#resources-ensure-lines)) may steer its enemies with scripts written in [Luau](https://luau.org), the language derived from Lua 5.1 that Roblox made for untrusted code. A script registers a **behaviour**, the named piece of logic a sheet chooses with `pilot`, exactly like the behaviours written in C++ (`straight`, `reaper`, `bydomancer`): it reads the same view of its body each tick and returns the same intent, and it may also read and write some components of its body through its handle. Changing a script needs no build: the next game reads it again.

Scripts run on the server only. They never reach the client, which draws the enemies from their sheets as before, so the authority of the server is unchanged. This page tells a designer what a script may read, return, hear and publish; the code is in `src/engine/Scripting/` (the machine, no R-Type word), `src/game/Scripting/` (the behaviours), `src/game/Scripting/Events/` (their access to the event bus, which knows nothing of the behaviours) and `src/game/Scripting/Level/` (the levels a script directs).

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

## What the function receives: the handle `e`

The function receives `e`, the handle of its body, and `behaviours.wait` returns the same handle. During the step of its body, `e` reads and writes the components of the list below, live: a value written is in the component at once, and a later read sees it. `e.data` is a table of the script's own, kept for the life of the body. Every other key reads the view of the step, below.

```lua
behaviours.register("molting", {}, function(e)
	e.data.molts = e.data.molts or 0
	if e.health <= e.full_health / 2 and e.data.molts == 0 then
		e.data.molts = 1
		e.collision_box = { width = 48, height = 48 }
		e.health = e.full_health
	end
	return { direction = { x = -1, y = 0 } }
end)
```

| Key | Component | Reads | A script may write |
|---|---|---|---|
| `position` | `Position` | the center of the body, `{x, y}` in logical units | two finite numbers |
| `health` | `Health` | the health points it has left | a whole number from 1 to `full_health` |
| `full_health` | `Health` | the points it appeared with | nothing: read only |
| `collision_box` | `CollisionBox` | its size, `{width, height}` in logical units | two positive finite numbers; the offset of the box from the center, which a boss sheet may give, stays |
| `top_speed` | `SteadyMotion` | its speed at a full direction, in logical units per second | a positive finite number |
| `score` | `ScoreValue` | the points the player who destroys it earns | a whole number from 0 to 4294967295 |
| `gun_interval` | `Gun` | the ticks between two of its shots | a whole number from 1 to 4294967295 |

A component the body does not have reads `nil`. Any other write is refused with an error at the line of the script that made it, and writes nothing: a value out of the bounds of the table, a component the body does not have, `full_health`, a key off the list, or `e.data` itself (write in it instead). An error a script does not catch with `pcall` stops its body, as any failure does (see [What a script cannot do](#what-a-script-cannot-do)).

The components of a body are only reached during its own step: the handle of another body, kept in a global, raises an error there. `e.data` is never sent to the clients, ends with the body, and shares the memory of the scripts of its game; the handle a script kept of a body that ended refuses every key. The list and its bounds are in `src/game/Scripting/Components/LuauComponentConstants.hpp`.

### The view

The view is rebuilt for each step and cannot be written. Its `position` and `top_speed` are those of the step's start; during the step, `e.position` and `e.top_speed` read the components instead, whenever the body has them.

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
| `health_share` | the share of its health points left, 0 to 1 |
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

The collision box, the top speed and the look stay until a later tick asks for others; an effect plays at each tick that names it, a quarter of a second (`EFFECT_REST`, 15 ticks) after the last one the body played at the earliest: an effect asked for sooner is left out, so one body announces four effects a second at most. An `effect` or a `look` must be a name of the content, 1 to 24 bytes of `a` to `z`, digits and `_`, otherwise it is left out; a name the sheet of the body does not hold is left out too, silently (see [Named effects and looks](/R-Type/entity-sheets/#named-effects-and-looks)). The script of the example asks for its speed every tick, so its Drifter charges only while a ship is level with it.

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
| `behaviours.wait(ms, intent)` | stops the function there: the body wants `intent` on every tick of the wait, its one-time requests (`starts_action`, `ends_action`, `jumps_to`, `effect`) on the first tick only, and the call returns the handle again, holding a fresh view, on the tick the wait ends. The wait lasts `ms` milliseconds rounded to the nearest tick (16.7 ms), one tick at least; a waiting body costs no call. It only runs in the function of a body; `coroutine.yield` called directly is a failure. |
| `behaviours.spawn(type, {x, y}, options)` | asks for a body of the sheet `type` at `{x, y}`, and returns its network id at once. It appears at the end of the tick, after every body was steered, and every player is told. A sheet that does not exist or that ships are made from is refused, and so are more than 256 bodies in one tick (`MAXIMUM_SPAWN_ORDERS_PER_TICK`). `options` may be left out; its `health_percent`, a whole number from 1 to 100, takes that share of the health of the sheet, rounded down and never under one point, as for a boss that comes as a mini-boss. A sheet with a `[gameplay.boss]` table needs nothing more: the players are warned of it and it enters `BOSS_ARRIVAL_WARNING` (4400 ms) later (see [Server](/R-Type/server/#boss-bosssystem-and-bossstage)). |
| `behaviours.destroy(id)` | removes the body of that network id at the end of the tick, after the bodies created in it; an id no body has does nothing, a ship is never removed, and more than 256 removals in one tick are refused (`MAXIMUM_REMOVAL_ORDERS_PER_TICK`). |
| `e.id` | the network id of the body being steered, the one `spawn` returned for it. |

Both `spawn` and `destroy` only run during a game: a script that calls them while it is first read fails.

## Hearing and publishing events

A script reaches the event bus of the game (see [Events](/R-Type/engine/#event-bus-eventbus)) through the table `bus`, so that the parts of a boss and its core talk to each other without sharing a line of code.

```lua
behaviours.register("serpent_segment", {}, function(view)
	local head = nil
	bus.on("serpent_head", function(event) head = event.id end)
	bus.on("entity_destroyed", function(event)
		if event.id == head then
			behaviours.destroy(view.id)
		end
	end)
	while true do
		view = behaviours.wait(100, { direction = { x = -1, y = 0 } })
	end
end)
```

| Function | Does |
|---|---|
| `bus.on(name, function)` | subscribes `function` to the event `name`, for the body that steps. The function receives one table, the event, and what it returns is ignored. It only runs in the function of a body, during a game. |
| `bus.emit(name, values)` | publishes an event under a name the script chooses, with `values`, a table of numbers, booleans and strings under string keys, 16 at most (`MAXIMUM_SCRIPT_EVENT_FIELDS`), or nothing. The values are copied: the functions that hear the event read a table of their own. It only runs during a game. |

Only these events of the game are shown to the scripts; any other name is an event the scripts publish themselves, and a script cannot publish under one of these four names.

| Name | Published when | The event holds |
|---|---|---|
| `entity_destroyed` | an entity is destroyed | `id`, its network id; `killed`, `false` when it was removed without an explosion |
| `entity_damaged` | an entity is hit and stays alive | `id` |
| `enemy_action_started` | an enemy enters a phase of an attack | `id`, `attack`, `phase` as in the view |
| `boss_health_changed` | the fight of a boss starts, a hit takes points from it, or it enrages | `id`, `points` left, `full`, the points it appeared with, `enraged` |

What a script may rely on:

- **The order is the one of the bus.** `bus.emit` only queues the event. The game loop delivers the events at the end of the tick, in the order they were published, each to its subscribers in the order they subscribed, C++ code and script functions alike. An event published by a subscribed function is delivered in the same pass.
- **A subscription ends with its body**, whichever way the body is destroyed, and when its script stops.
- **Subscribe once.** A function that returns starts again at the next tick, and would subscribe again on each of them: a body keeps 16 functions at most (`MAXIMUM_SUBSCRIPTIONS_PER_ENTITY`), and one more is a failure. Subscribe before a loop that waits, as the example does. A subscribed function shares the local variables of the function that subscribed it, which is how it tells the body what it heard.
- **A subscribed function is a step.** It has the 2 ms of a step and counts in the 8 ms of the tick. It cannot wait. One that fails, waits or runs past its budget ends every subscription of its body at once, and stops the script of that body at its next tick, as a failed step does: the journal writes it as the first failure of that behaviour. The functions subscribed after it are still called.
- **Once the budget of a tick is spent, the events left reach no script function** until the next tick; they are not kept. This is also what ends two functions that answer each other without end.

## Judging a hit before it applies

A body may decide, before the server announces it, whether a missile that touches it hits it: a shield, a weak point, armor that turns shots away.

```lua
behaviours.register("shielded_core", {}, function(e)
	local shield = 3
	behaviours.pre_hit(function(hit)
		if shield > 0 and hit.offset.x < 0 then
			shield = shield - 1
			behaviours.destroy(hit.missile)
			return false
		end
	end)
	while true do
		behaviours.wait(1000, { direction = { x = -1, y = 0 } })
	end
end)
```

`behaviours.pre_hit(function)` makes `function` judge every hit of a missile on the body that steps, in place of the one declared before; `behaviours.pre_hit(nil)` forgets it. It only runs in the function of a body. The function receives the hit, `missile` the network id of the missile and `offset` where the center of the missile lies from the center of the body, `{x, y}` in logical units, read in the present even for a missile the server tests where its player saw the body. Returning `false` cancels the hit: no damage, nothing announced, and the missile flies on, so a shield that should stop it removes it with `behaviours.destroy`. Any other value lets the hit apply. A cancelled missile that still touches the body is judged again at the next tick.

- **A body without a `pre_hit` behaves as before**: asking it costs one lookup and no call of a script.
- **It is a step without the handle**: it has the 2 ms of a step and counts in the 8 ms of the tick; `e` refuses every key there, as in a function subscribed to an event. It cannot wait.
- **Once the budget of the tick is spent, the hit applies** without a call.
- **A failure lets the hit apply**: a function that fails, waits or runs past its budget is forgotten, and stops the script of its body at its next tick, as a failed step does.

## Reacting to a hit once it applied

The sheet of a body may name a function told of every hit of a missile on that body once the server applied it: a wall that cracks, a crate that drops a bonus, a body that changes its look below half its health.

```lua
behaviours.on_hit("crack", function(hit)
	if hit.destroyed then
		bus.emit("wall_broken", { id = hit.id })
	end
end)
```

```toml
[gameplay]
side = "obstacle"
layer = "wall"
pilot = "straight"
on_hit = "crack"
```

`behaviours.on_hit(name, function)` declares a hit handler while the script is read, as `behaviours.register` declares a behaviour; two handlers of one name stop the supervisor at start (`DuplicateHitHandlerException`), and a sheet that names a handler no script declared is refused. The handler serves every body whose sheet names it, any side but `player`, and whatever pilots the body, a built-in behaviour included. `DamageRules` calls it after the hit applied and its announcements were published, a hit on a body without `health` included, so what the function publishes arrives after them. It receives `id` the network id of the body, `missile` that of the missile, `damage` the health points the hit took (0 for a body without `health`, or a boss arriving), `health` the points left, absent for a body without `health`, and `destroyed`. It cannot undo the hit, and may create and remove bodies.

- **It is a step without the handle**: it has the 2 ms of a step and counts in the 8 ms of the tick; `e` is not given, and it cannot wait.
- **Once the budget of the tick is spent, it is not called**: the hit still applies.
- **A failure is reported once**, as `the on_hit function <name> failed: <reason>`; the hit stays applied, no body is stopped, and later hits still call it.

## Directing a level

A script may also write a level: what comes, when the decor moves and stops, which biome the players see and where a lost team starts again. A server plays it with the setting `level` (see [Server](/R-Type/server/#level-a-script-directs-leveldirector)); without it, a game plays the waves of its difficulty.

```lua
level.register("training_run", {
	function()
		level.spawn_placed()
		level.wait_scroll(1264)
		level.pause_scroll()
		while level.enemies() > 0 do
			level.wait(250)
		end
		level.resume_scroll()
		behaviours.spawn("drifter", { x = 1984, y = 540 })
	end,
	function()
		level.biome("fosse")
		level.wait_scroll(1680)
	end,
}, { ldtk = "levels/training_run.ldtk" })
```

`level.register(name, stages, options)` takes the name the setting gives, a list of 1 to 64 functions (`MAXIMUM_LEVEL_STAGES`), the stages, run one after the other, each in a thread of its own, and an optional table of options: `ldtk` names the LDtk file that places the enemies of the level (see [Placing the enemies in LDtk](#placing-the-enemies-in-ldtk)), and `starts` maps a stage, from 2 to the last, to where it starts, `{ scroll = …, biome = …, biome_since = … }` in logical units, `biome_since` its `scroll` when left out and `biome` none, so a game can start there with `--stage` (see [Server](/R-Type/server/#launch-options-launchoptions)), and `bands` maps a biome name to the rows a ship flies between while the level shows it, `{ top = …, bottom = … }` in logical units from the top of the playfield, from 0 to the bottom of the play area (984) and at least a ship (64) apart: the ceiling and the floor of a biome whose decor closes the sky, which the server stops every ship at and the client predicts against. A biome the option does not name leaves the whole play area. `biome` names the biome the level opens in: its players start inside it, its entrance already behind them, since the server tells it began `OPENING_BIOME_LEAD` (4096 logical units) before the scroll starts, so a client never plays its entrance. `map` makes the level a map the waiting room votes for, under the name it gives: 1 to 16 capital letters, digits and spaces (`MAXIMUM_MAP_NAME_SIZE`); a server without the setting `level` offers every map its scripts register, at most 8, in the order of the names of their levels, and each game plays the one the vote chose (see [Protocol](/R-Type/protocol/#map-vote)). `preview` names the image the waiting room shows for that map: a content name (1 to 24 lowercase letters, digits and underscores), the file `previews/<name>.png` of the resource, 80 × 45 pixels, which the content pack carries to the clients. A name already registered, an empty name, a stage that is no function, an option no level has, an `ldtk` path that is absolute or leaves its folder with `..`, a band out of those bounds, a `biome` that is no biome name a `map` name out of those rules, or a `preview` that is no content name or given to a level without `map`, stops the supervisor at start. The level scrolls from 0 at 120 logical units per second (`LEVEL_SCROLL_SPEED`), and the events of a level are best hung on that scroll, so everything after a pause waits with it.

| Function | Does |
|---|---|
| `level.wait(ms)` | stops the stage for `ms` milliseconds, rounded to the nearest tick, one tick at least, as `behaviours.wait` does. It only runs in a stage. |
| `level.wait_scroll(position)` | stops the stage until the level scrolled `position` logical units; while the scroll is paused short of it, the stage waits. It only runs in a stage. |
| `level.scroll()` | how far the level scrolled, in logical units. |
| `level.pause_scroll()`, `level.resume_scroll()` | stop the scroll where it is, and let it move on. The decor of the players stops and moves with it. |
| `level.biome(name)` | names the biome the players see from now: 1 to 24 lowercase letters, digits and underscores. A client shows the background named after it, a file of its `assets/backgrounds/` (see [Writing a background](/R-Type/backgrounds/)), and the sky of open space for a name it has no file for. A new name begins where the scroll stands: a background that has an entrance arrives from the right edge of the screen from there, through that entrance; naming the biome already shown changes nothing. |
| `level.enemies()` | how many enemies the world holds, the bodies of side `enemy`. |
| `level.placements()` | the enemies the LDtk file of the level places, sorted by `x`: a list of tables `{ entity, sheet, x, y, width, fields }` (see [Placing the enemies in LDtk](#placing-the-enemies-in-ldtk)); empty for a level without an `ldtk` option. |
| `level.transition(ms)` | pauses the scroll and plays a transition of `ms` milliseconds, 3000 to 60000 (`SHORTEST_LEVEL_TRANSITION`, `LONGEST_LEVEL_TRANSITION`), between two parts of the level, then stops the stage until it ends, when the scroll moves on. While it lasts, the server holds every ship: no input reaches it, it does not move, fire, roll or dash, and it is invulnerable. The client flies the ships out to the right for 900 ms with the warp of the stars, keeps them away, and flies them back in from the left for the last second, the server putting them back at their start as it begins (`LEVEL_TRANSITION_FLY_OUT`, `LEVEL_TRANSITION_AWAY`, `LEVEL_TRANSITION_FLY_IN`, in `src/game/Level/LevelConstants.hpp`). Each `LevelStatus` tells the time left, so a player who joins during one plays its end. A restart from a checkpoint forgets it. It only runs in a stage. |
| `level.spawn_placed()` | from now on, each placed enemy appears as the scroll brings its left edge to the right edge of the playfield, for the rest of the level, restarts from a checkpoint included; calling it again changes nothing. It only runs during a game. |

What a script may rely on:

- **A stage steps before the bodies.** Each tick, the stage due runs first, then the placed enemies due come in, then the level scrolls, then the bodies are steered, so a body a stage creates with `behaviours.spawn`, or a placed enemy, appears at the end of that tick. A stage that waits costs nothing; one that runs has the 2 ms of a step.
- **Each stage after the first starts at a checkpoint.** When a stage returns, the next one starts at the following tick, and the place it starts from is kept: the scroll and the biome of that moment, with where that biome began. A team that loses every life past a checkpoint starts that stage again from its first line, at that scroll and in that biome, the scroll moving, with every enemy and missile removed and the lives of the rules given back; of the placed enemies, only those that come in after the checkpoint come back. Before the first checkpoint, the game is lost.
- **The level is cleared once its last stage returned and no enemy is left.** The game is then won. A stage that fails, yields other than through `level.wait` or `level.wait_scroll`, or runs past its budget ends the game as lost, and the journal says why once.
- **A boss comes as any body does.** A stage creates it with `behaviours.spawn`, past the right edge, such as `behaviours.spawn("bydo_tyrant", { x = 2176, y = 492 }, { health_percent = 40 })`: the players get the WARNING once it appeared, nothing steers it for `BOSS_ARRIVAL_WARNING` (4400 ms), then its health is scaled for the players connected and the `boss_health_percent` of the difficulty and it enters. `level.enemies()` counts it from the tick after the stage created it. Several bosses may come in one level: each plays the whole arrival again on the client, the alarm, the WARNING, the music of the boss, then its bar. In a game that plays a level, the music of the game comes back once that boss is defeated; in a game of waves, the music of its boss plays until the game ends. A team that loses every life during a warning starts the stage again, and the boss of that warning is removed with every other body.
- **Any script may read and move the level during a game that plays one.** The behaviour of a boss may call `level.pause_scroll()` when it arrives. In a game that plays its waves, every function of `level` but `register` raises an error, which stops the body that called it. `bus.on` only runs in the function of a body, so a stage hears no event: it polls, as the example does with `level.enemies()`.

The repository ships three levels; the two of the resource fosse are maps, each in one biome:

| Level | Resource | What it plays |
|---|---|---|
| `space` | `resources/fosse/` | the map OPEN SPACE: two stages in open space, with the stars alone, the second ending on the Bydo Tyrant |
| `fosse` | `resources/fosse/` | the map THE PIT: it opens inside the Pit (`biome = "fosse"`), where only the mobs of the Pit come, in three stages, the last one on the boss of the Pit, the Necromancer. It declares the starts of its second and third stages, `--stage 2` in its depths and `--stage 3` on the Necromancer, and the band of the Pit, its ceiling on row 192 and its floor on row 848, which no ship flies past (see [The mobs of the Pit](#the-mobs-of-the-pit-and-the-necromancer)). The positions of its enemies are written in the script until a level places them in its LDtk project. |
| `training_run` | `resources/training-run/` | two stages, fifteen enemies placed in `levels/training_run.ldtk`, a pause of the scroll on each Reaper until it is destroyed, a checkpoint, the stars and then the biome `fosse` |

With `ensure fosse` alone in `server.cfg`, the waiting room votes between OPEN SPACE and THE PIT; `set level fosse` plays the Pit with no vote. Play the training run with `ensure training-run` and `set level training_run`.

### The mobs of the Pit and the Necromancer

The resource fosse brings its own enemies: their sheets in `entities/`, their sprites in `sprites/fosse/`, and the behaviours that steer them in `server/fosse_mobs.luau`, `server/gloomwyrm.luau` and `server/necromancer.luau`. Every mob but the Gloomwyrm crosses the playfield from the right to the left, some along a wave, and plays its idle loop while it moves; an attack is three actions of attack 0, `signal`, `strike` and `recover`, whose lengths in the sheet are those of their animations.

| Sheet | Pilot | What it does |
|---|---|---|
| `sporecap` | `sporecap_walk` | walks left along the bottom row; a ship within `reach` of its column, above it, makes it puff `spores` `sporecap_spore` missiles, `spore_gap` units apart, which weave left |
| `lanternfin` | `lanternfin_hunt` | swims left along a wave (`curve`, `curve_ms`), drawing towards the row of the nearest ship; that ship ahead within `reach` and `align` units of its row, its lure flashes and it lunges at `lunge_speed` |
| `stalagmaw` | `stalagmaw_drop` | crawls left along the top row at `crawl_speed`; a ship below it within `reach` makes it shake, then fall at `fall_speed`, and it is removed below the playfield |
| `ruin_warden` | `warden_float` | floats left, bobbing (`bob`, `bob_ms`); left of `fire_from_x` and rested, its rune lights up and it fires one `ruin_warden_bolt` |
| `glowmoth` | `weave` | flutters left in a wave, in swarms; it does not attack |
| `gloomwyrm` | `gloomwyrm_head` | a serpent of bones: its skull spawns `length` bodies in a line behind it, `gloomwyrm_segment` vertebrae and a `gloomwyrm_tail`, swims left along a wave (`wave` units down and back every `wave_ms`, `waves` times), turns back along a half circle of `radius` units, down when it entered above row 520 and up otherwise, and swims out to the right along a wave. Its skull is the weak point: once it is killed, every body left bursts in turn, from the neck to the tail. Each of its three sheets draws its own death, a burst of bone, in place of the Bydo explosion |
| `gloomwyrm_segment`, `gloomwyrm_tail` | `gloomwyrm_body` | follow the path of the skull, each `LINK_TICKS` (10) ticks of path behind the body before it, so the wyrm moves as one; each is shot down on its own (3 points of health for a vertebra, 6 for the tail), and the wyrm swims on without it; once the skull is killed, each plays its `burst` effect `BURST_TICKS` (5) ticks after the body before it, and is removed. A skull that swims out of the playfield is removed without a kill, and its bodies swim out after it |

The Necromancer (`necromancer`, pilot `necromancer_rite`) is the boss of the last stage. It appears below the screen, out of sight through the warning, then moves to where it rests and rises out of the floor over `rise_ms` (its arrival, attack 5, whose frames are cut at the floor), then plays its attacks in place: the rings (attack 0), the fist (1), the raise of the dead (2) and the gaze (3), each once in that order and then at random. A missile that touches its hood or its cape hurts it; its `pre_hit` stops every missile while it comes up or hides in its hood. Its bar writes NECROMANCER, the `boss_name` of its sheet (see [Writing an entity sheet](/R-Type/entity-sheets/#appearance-key-by-key)). Below half its health it hides in its hood for `rage_ms` (attack 4) and pairs its attacks: rings with the fist, skeleton arms under the gaze. What leaves it is a body of its own, which a ship touches and which stops missiles or lets them through:

| Sheet | Pilot | What it does |
|---|---|---|
| `necromancer_ring` | `necromancer_ring` | flies left along its lane to `turn_x`, sweeps to `middle_y`, flies back right and is removed at `return_x`; it stops missiles |
| `bone_spike` | `necromancer_effect` | eight of them erupt one after the other along the floor, from the impact of the fist to the left edge, 120 units and 90 ms apart, and each hurts during its strike |
| `bone_arm` | `necromancer_effect` | opens a fissure for `strike_after_ms`, then bursts up and hurts during its strike; three of them rise, moved together each time |
| `necromancer_beam` | `necromancer_gaze` | comes with the signal of the gaze and shows its look `aim` for `aim_ms`, a thin pink line that blinks where the beam will burn, then burns for `burn_ms`, one sprite 1600 units long from the mask to the left edge, and is removed; the beam that hurts is the `hurt_zone.beam` of the Necromancer, during the strike of the gaze |

The fist hurts through a box of the Necromancer's `hurt_zone`, the first 333 ms of its strike. A body of `necromancer_effect` or `necromancer_gaze` keeps its hitbox 1024 units below its center, out of the playfield, so only its hurt zone touches a ship. The values of these sheets are **provisional** until gameplay tunes them.

### The Ashlands

The resource ashlands brings the map THE ASHLANDS, the level `ashlands` of four stages, each in a biome of its own named with `level.biome`: Highway Zero (`cendres`, no ceiling), Shelter 9 (`cendres_abri`), the core (`cendres_coeur`) and Silo Zero (`cendres_silo`, entered through a `level.transition` of 4 s). Its sheets are in `entities/`, its sprites in `sprites/cendres/`, its scripts in `server/`: `ashlands.luau` directs the level, `ashlands_mobs.luau` steers the mobs, `ashlands_props.luau` the decor, and `sandmaw.luau`, `glowheart.luau` and `ash_mother.luau` the three bosses. `--stage 2`, `3` and `4` start in the shelter, at the Glowheart and in the silo. Every mob crosses from the right to the left and attacks in the three actions of attack 0, as in the Pit:

| Sheet | Pilot | What it does |
|---|---|---|
| `cindermite` | `weave` | flutters left in swarms; it does not attack |
| `dunecrawler` | `dunecrawler_walk` | walks left along the floor; a ship above it within `reach` of its column makes it fire a `dunecrawler_barb` straight up (`rising_barb`) |
| `scrapdrone` | `scrapdrone_patrol` | drifts left, bobbing; left of `fire_from_x` and rested, it fires a `scrapdrone_bolt`, aimed at the nearest ship (`aimed_bolt`) |
| `rail_turret` | `rail_turret_slide` | slides left along the ceiling; a ship below it within `reach` makes it fire `shots` `rail_turret_shot` down, `shot_gap_ms` apart (`falling_shot`) |
| `geiger_mine` | `geiger_drift` | drifts left; a ship within `trigger` units makes it tick `signal_ms`, then burst into a volley of eight `geiger_mine_shard` (`scatter`), and it is gone; shot before, it only pops |
| `fixbot` | `fixbot_roll` | rolls left on the floor; a ship ahead within `reach` makes it throw a `fixbot_wrench` along an arc (`thrown_wrench`) |
| `brood_mite` | `brood_swarm` | flies left, turning `turn` percent of the way towards the nearest ship |

A missile a script steers (`rising_barb`, `falling_shot`, `aimed_bolt`, `thrown_wrench`, `blast_life`) is steered once when its sheet is read, with no ship in view, and must then head towards the edge that removes it (see [Writing an entity sheet](/R-Type/entity-sheets/#rules-between-keys-and-between-sheets)): each one reads only its settings and its age on that call.

The three bosses each have a `[gameplay.boss]` table and a `boss_name`. The Sandmaw (`sandmaw_maw`) closes the highway: it appears where it rests by the right edge and comes up out of its dune (attack 2, its frames cut at the top of the dune, as the Necromancer rises), then stays planted there, its maw at floor level; it opens its maw (`open_ms`) and either inhales, a `sand_debris` from the left edge every `debris_ms` for `inhale_ms`, each flying straight into its maw (`swallowed_debris`), shootable and swallowed once it arrives, or spits `boulders` `sandmaw_boulder` along an arc (`thrown_wrench`) and a Dunecrawler every second spit; its `pre_hit` lets a hit through only while its maw is open, on its rows. Both draw attack 0, the only one its sheet holds. The Glowheart (`glowheart_core`) slides to `rest_x`, then plays its Tendrils (attack 0: two `glowheart_tendril` sweep two of three lanes, the third safe), its Fallout rain (1: two waves of `glowheart_drop` with two free columns) and its Meltdown (2: a `meltdown_beam` aims, then the `hurt_zone.beam` of its sheet burns its row); its `pre_hit` lets a hit through only during the `purge_ms` that follows a Meltdown, on the rows of its core window. The Ash Mother (`ash_mother_brood`) appears where she rests and climbs out of the rubble of the silo (attack 3, its first frames cut at the top of the mound), then plays her Scythes (0, a box of her `hurt_zone`), her Atomic breath (1, an `atomic_breath` and her beam) and her Brood (2, `brood` `brood_mite`, `enraged_brood` once enraged); her `pre_hit` lets a hit through only while her maw is open, from the signal of the breath to its recover, on its rows. Both put a Meltdown or a breath every third attack.

The decor that does something is a body the level creates past the right edge, moved by the scroll (`motion.kind = "scroll"`), never a part of the maps:

| Kind | Sheets | What it does |
|---|---|---|
| Hazard (`hazard_cycle`, `steam_vent_cycle`) | `rad_geyser`, `live_cable`, `control_rods`, `steam_vent` | an enemy of the layer `hazard`, its hitbox 1024 units below it, that hurts through its `hurt_zone` during the strike of a cycle; a steam vent also jets for as long as the Glowheart purges |
| Breakable (`breakable_watch`) | `toxic_barrel`, `billboard`, `vent_grille` (`vent_spawner`) | an enemy with health; the hit handler `barrel_burst` lays four `toxic_blast` and a `toxic_puddle`, `billboard_fall` a `billboard_wreck`, missiles of the layer `hazard_blast` that hit the layer `enemy` only, so they hurt the mobs; a grille lets a Cindermite out every `interval_ms` |
| Trigger (`searchlight_sweep`, `alarm_wake`) | `searchlight`, `alarm_beacon` | an obstacle of the layer `decor`, which nothing hits; the first ship caught in the beam calls `drones` Scrapdrones, and the beacon, past `wake_x`, sends `fixbots` Fixbots |
| Wall | `blast_door`, `blast_door_lower` | two blocking obstacles that leave a gap of 260 units around the row the level gives |
| Decor (`coolant_drip`, `meltdown_lamp_watch`) | `coolant_leak`, `meltdown_lamp` | a leak drips `glowheart_drop`; both light up with the signal of the Glowheart they hear on the bus |

The Glowheart publishes `glowheart_rain`, `glowheart_meltdown` and `glowheart_purge`, each with `ms`, the length of what it announces. A hazard learns the id of the boss on screen from `boss_health_changed` and removes itself once that boss is killed, so a fight whose scroll is paused ends with the boss. The values of these sheets are **provisional** until gameplay tunes them.

## Placing the enemies in LDtk

A level places its enemies in [LDtk](https://ldtk.io), on a layer of entities named `Mobs`, instead of writing each position in its script. The script names the file with the option `ldtk` of `level.register`, a path relative to its content folder, `levels/<level>.ldtk` by convention; a resource may also lay its file over one of the assets or of a resource started before it, the last folder that holds it winning. Each game process reads the file when it starts, after the entity sheets, and refuses to start on a file that is missing, that is not JSON, whose levels are saved in separate files, that has no `Mobs` layer of entities, or that places an entity the game cannot read: the error names the file, the entity and where it is placed (`the entity Ghost placed at 2224, 300 names no sheet ghost`), and the supervisor goes on. The reader is `ldtkPlacementsOf()` (`src/game/Level/Ldtk/`, `rtype_game_level_ldtk`), which reads the JSON with `nlohmann-json`.

```
resources/training-run/
├── resource.toml
├── levels/training_run.ldtk       the Mobs layer: 15 Drifters, Weavers, Reapers and Voltjellies
└── server/training_run.luau       registers training_run with { ldtk = "levels/training_run.ldtk" }
```

Each entity of the layer gives one placement:

- **The identifier names the sheet.** Each capital letter after a letter or a digit starts a word, the words are joined by underscores and the letters are lowercased: `Lamprey` places a `lamprey`, `BydoViolet` a `bydo_violet`. The sheet must exist and be one a behaviour pilots: a ship is refused.
- **One LDtk pixel is 4 logical units** (`LDTK_PIXEL`): a level of 480 by 270 pixels is the 1920 by 1080 playfield, and one of its pixels is one pixel of the canvas of the client. The center of the entity is read from its pivot (`px + (0.5 - pivot) × size`), so a pivot at the top left or at the center places the same box. When a file holds several levels, they follow one another from left to right in the order of the file, the first starting at 0: `x` is the start of the level plus the pixel, times 4; `y` is the pixel times 4. `worldX` is not read.
- **The custom fields come along**: `Int` and `Float` as numbers, `Bool` as booleans, `String`, `Multilines`, `Enum(...)` and `Color` as text; a field whose value is null is left out, and a field of another kind (a point, a tile, a reference, an array) is refused with its name.

`level.placements()` lists them sorted by `x`, each as `{ entity = "Reaper", sheet = "reaper", x = 3184, y = 540, width = 96, fields = {} }`, in logical units of the level. `level.spawn_placed()` asks that they appear: each one once the scroll brings its left edge to the right edge of the playfield, at `x - PLAYFIELD_WIDTH - width / 2` units of scroll (an enemy placed on the first screen appears at its place at the first tick), with its center at `x - scroll`, `y`. It follows the pauses of the scroll, since it is the scroll that brings them in, and a restart from a checkpoint only brings back the enemies that come in after it. More than 256 placed enemies due in one tick (`MAXIMUM_SPAWN_ORDERS_PER_TICK`) make the game throw: a level places fewer in one step of the scroll. The script keeps the rest: biomes, pauses, checkpoints, and any enemy it still creates itself with `behaviours.spawn`, as the example above does once the sky is clear.

## What a script cannot do

- **Reach the system.** The sandbox of Luau removes `io`, `package`, `require`, `dofile`, `loadfile` and `load`, leaves `os` its clock and dates only, and makes every standard library read-only; R-Type also removes `getfenv` and `setfenv`. Each script has globals of its own: a global of one script is not seen by another.
- **Hold the server.** All the scripts of a game share 16 MiB (`SCRIPT_MEMORY_LIMIT_BYTES`), and the machine collects every garbage at the start of a tick once they hold 75 % of it. Reading a script has 20 ms (`SCRIPT_CALL_BUDGET`). Each step of a body, from one wait to the next, has 2 ms (`SCRIPT_STEP_BUDGET`), and the steps of one tick have 8 ms together (`SCRIPT_TICK_BUDGET`): once those are spent, the bodies still due keep what they wanted and step at the next tick, and the journal of the game says so once. A step is stopped only after 1024 loops or calls past its deadline (`SCRIPT_OVERDUE_SAFEPOINTS`), so a pause of the machine does not stop a body that did nothing wrong.
- **Survive a failure.** A step that fails, runs past its budget or asks for too much memory stops the script of that body for good: the body is removed from the game at the end of the tick, so a body frozen out of reach cannot keep a level from ending; the other bodies go on, and the journal writes the first failure of each behaviour once.
- **Draw something its sheet does not.** A script names an effect or a look its sheet draws; a new animation, a sound, or an effect outside the sprite sheet of the type belongs to the sheet and the client, not to a script.

## Checking a script before a game

`luau-analyze`, the analyzer of Luau, reports syntax errors and, with `--!strict` at the top of a file, the values a script uses wrongly; the example script starts with it. The project does not ship a definition of `behaviours` for it yet, so the analyzer may report `behaviours` as an unknown global: that line is expected. This check was not run for this page.

## What a script costs

`script_budget_bench` (`tests/tools/ScriptBudgetBench/`) measures what the scripts take of a tick, so that a level is written knowing how many scripted bodies the server holds. It keeps alive 300 bodies steered by the script of the Crimson Drifter (`--scripted <count>`), 1000 bullets flown by the built-in behaviour `straight` (`--bullets <count>`) and four ships, and computes 3600 ticks of 16.7 ms (`--ticks <count>`), one after the other with no wait. Each tick runs `PilotSystem`, `MovementSystem`, `PlayfieldExitSystem` and `CollisionSystem`, in the order of a game, then replaces the bodies that left the playfield, so the load stays the same. The bodies are spread over 16 rows and the bullets over 32 rows of their own, so the bullets are not stacked against each other. Nothing is random, so two runs do the same work.

```bash
cmake --preset bench
cmake --build --preset bench --target script_budget_bench
./build-bench/tests/script_budget_bench
```

The `bench` preset is the `test` preset in Release: a build that is not optimized says so on its error output, since its figures say nothing of a server. The report gives the median, the 99th percentile and the worst tick, the same for the scripts alone, the same for `CollisionSystem` alone, the share of the scripts and of the collisions in the time of all the ticks, the collisions found, and how many ticks passed 16.7 ms. The time of the scripts is read around each call, garbage collection included; the two clock reads of a call count in it (14 ns each, about 3 % of a call).

`--waiting` runs the same bodies with a script that waits one tick at a time and never returns, so each tick resumes every coroutine: the most a waiting body can cost.

Measured on an Apple M4 Pro with Apple clang 21 over 3600 ticks, before the bench ran `CollisionSystem`: three runs for the first row, two for the second, one for the third, three for the last.

| Scripted bodies, bullets | Median tick | Worst tick | Median of the scripts | Share of the scripts |
|---|---|---|---|---|
| 300, 1000 | 0.74 to 0.75 ms | 1.3 to 4.5 ms | 0.27 ms | 36 % |
| 3000, 1000 | 4.3 to 4.4 ms | 6.0 to 14.2 ms | 2.7 to 2.8 ms | 63 % |
| 6000, 1000 | 8.3 ms | 14.0 ms | 5.5 ms | 67 % |
| 300 waiting, 1000 | 0.77 to 0.78 ms | 1.7 to 4.9 ms | 0.27 ms | 36 % |

A call of the script takes 0.9 µs, and the time grows in line with the bodies from 300 to 6000. The medians repeat within 2 %; the worst tick varies from run to run, since it depends on what else the machine does. These figures hold for this script, which reads a few keys and returns one table, and for a tick without collisions, weapons or the sending of the world state: they are the cost of the scripts, not of a whole game tick.

### What the collisions cost

`--cell-side <units>` sets the side of the cells of the grid `CollisionSystem` sorts the boxes into (see [Detection](/R-Type/server/#detection-collisionsystem)); it defaults to the side the game uses, `COLLISION_CELL_SIDE`. A side the grid refuses, 0 or under 7.5 units, stops the bench with the error of the grid. The time of `CollisionSystem` is read by the scheduler around the system, publishing its collisions included.

Measured on an Apple M3 Pro with Apple clang 21 over 3600 ticks, 300 scripted bodies and 1000 bullets, three runs for each side:

| Cell side | Median of the collisions | 99th percentile of the collisions | Share of the collisions | Median tick |
|---|---|---|---|---|
| 60 | 0.161 to 0.164 ms | 0.20 to 0.23 ms | 14.5 % | 1.11 to 1.13 ms |
| 120 | 0.189 to 0.193 ms | 0.23 to 0.25 ms | 16.6 to 16.7 % | 1.14 to 1.16 ms |
| 240 | 0.254 to 0.255 ms | 0.31 to 0.32 ms | 21.0 % | 1.21 to 1.22 ms |

The three sides found the same 9,457,143 collisions, about 2,600 a tick: 300 bodies of 96 by 96 cover more than the playfield, so nearly every bullet lies in a body. Cells of 60 cost 15 % less than cells of 120, far more than the 2 % the runs of one side differ by, so `COLLISION_CELL_SIDE` is 60, though a ship of 128 by 64 covers six to twelve such cells. The bench has no wall: a level with walls larger than a cell is measured again before the side is trusted for it.

## Where to intervene

| You want to… | Look at |
|---|---|
| Change what a script reads or may return | `pushViewTable()` and `intentFromTable()` in `src/game/Scripting/LuauBehaviourTables.cpp`, the keys in `GameScriptingConstants.hpp` |
| Change the components a script reaches, or their bounds | `pushComponentField()` and `writeComponentField()` in `src/game/Scripting/Components/LuauComponentFields.cpp`, the keys and bounds in `LuauComponentConstants.hpp` |
| Change what the handle `e` refuses, or what it keeps of a body | `LuauEntityHandle` in `src/game/Scripting/Components/` |
| Change the memory or the time a script has | `SCRIPT_MEMORY_LIMIT_BYTES`, `SCRIPT_CALL_BUDGET`, `SCRIPT_STEP_BUDGET` and `SCRIPT_TICK_BUDGET` in `src/engine/Scripting/ScriptingConstants.hpp` |
| Change what a body holds during a wait, and when it stops | `HeldIntent` in `src/game/Behaviours/Piloting/`, which knows nothing of Luau |
| Change how a Luau function is resumed after a wait | `LuauBehaviour` and `LuauBehaviourState` in `src/game/Scripting/` |
| Change how created or removed bodies reach the game | `IEntityOrders` in `src/game/Spawning/`, carried out by `EntityOrders` in `src/server/Spawning/` |
| Show another event of the game to the scripts | `GAME_EVENT_NAMES` in `LuauEventConstants.hpp`, then its `fieldsOf()` and its line of `subscribeNamed()` in `src/game/Scripting/Events/LuauEventBindings.cpp` |
| Change what a published event may carry | `ScriptEvent` and `ScriptEventField` in `src/game/Events/`, read by `publishedFields()` in `LuauEventBindings.cpp` |
| Change when a subscription ends | `LuauEventSubscription` and `LuauEventSubscriptions`, what one body keeps subscribed, in `src/game/Scripting/Events/` |
| Change what a pre_hit function receives, returns, or what its failure does | `LuauPreHit` and `LuauBehaviourState` in `src/game/Scripting/`, the hit table in `pushHitTable()` of `LuauBehaviourTables.cpp`; who asks it, `targetLetsHit()` in `src/game/Systems/CollisionRules/PreHitCheck.cpp` |
| Change what an on_hit function receives, or when it is called | `LuauHitHandler` in `src/game/Scripting/`, the hit table in `pushAppliedHitTable()` of `LuauBehaviourTables.cpp`; who calls it, `DamageRules::onMissileHit()` in `src/game/Systems/DamageRules/`; the call on a thread of its own, shared with pre_hit, in `LuauSingleCall` |
| Change what happens to a body whose subscribed function failed | `LuauBehaviour::steer()` in `src/game/Scripting/`, the only place that stops a script |
| Change what a level script may call, or what it refuses | the functions of `LuauLevels` in `src/game/Scripting/Level/LuauLevels.cpp`, their names and messages in `LuauLevelConstants.hpp` |
| Change when a stage is resumed, or what a checkpoint keeps | `LuauLevelRun` in `src/game/Scripting/Level/`, `LevelCheckpoint` in `src/game/Level/` |
| Change a mob of the Pit, the Necromancer or one of its attacks | its sheet in `resources/fosse/entities/`, its behaviour in `resources/fosse/server/fosse_mobs.luau` or `necromancer.luau`, when they come in `fosse.luau` |
| Change a mob, a piece of decor or a boss of the Ashlands | its sheet in `resources/ashlands/entities/`, its behaviour in `resources/ashlands/server/ashlands_mobs.luau`, `ashlands_props.luau`, `sandmaw.luau`, `glowheart.luau` or `ash_mother.luau`, when it comes in `ashlands.luau` |
| Change how an LDtk file is read, or what it refuses | `ldtkPlacementsOf()` in `src/game/Level/Ldtk/LdtkPlacementReading.cpp`, the members and reasons in `LdtkConstants.hpp`; the check of the sheets in `readLevelPlacements()`, `src/game/Scripting/Level/LevelPlacementFiles.cpp` |
| Change when a placed enemy comes in, or where it appears | `PlacedEnemySpawner` in `src/game/Spawning/`, ticked by `LevelDirector` in `src/server/Spawning/` |
| Expose another function to the scripts | `ScriptMachine::expose()`, before the first script runs, as `BehaviourScripts` exposes `behaviours.register` |
| Change which files are scripts | `scriptSourcesIn()` in `src/game/Scripting/BehaviourScriptFiles.cpp` |
