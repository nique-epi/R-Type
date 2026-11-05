---
title: 'Architecture decision: engine, game and network libraries'
sidebar:
  label: Architecture
---

## Context

The client renders with SFML, the server talks over the network with Asio (the reasons for these tools are in [Technology choices](/R-Type/technology-choices/)), and both must run the same game logic. If rendering, networking and logic live in one place, no part can be changed or tested alone, and the server would be forced to link a graphics library.

## Decision

The code is split into three libraries with one-way dependencies, and the game objects are organized as an **ECS** (Entity Component System).

```
r-type_client -> rtype_client_modules -> SFML
      |-> rtype_game    -> rtype_engine
      '-> rtype_network -> Asio

r-type_server -> rtype_game    -> rtype_engine
      '-> rtype_network -> Asio
```

| Library | Holds | Must not know |
|---|---|---|
| `rtype_engine` | entities, components, systems (generic ECS) | SFML, Asio, the game rules |
| `rtype_game` | R-Type rules built on the engine | sockets, the network protocol, SFML |
| `rtype_network` | Asio event loop, protocol, transport | the game rules, SFML |

The client and the server are the only places where the libraries meet: they translate between network messages and game state. `r-type_server` never links SFML. This is checked at configure time by `rtype_forbid_links` in the root `CMakeLists.txt`.

## Why an ECS

- Data is separate from behavior, which matches the engine/game split: the engine knows how to store and iterate, the game decides what a component or a system means.
- Client and server share the same components; only the systems differ (the client adds rendering and input, the server adds authority).
- A new enemy or power-up is a new combination of components, not a new class in a hierarchy.

Alternative considered: a classic class hierarchy of game objects with virtual `update()` and `draw()`. It is simpler at first, but ties logic to rendering, which the server cannot afford, and makes cross-cutting behaviors (a boss that is also a shooter) awkward.

| | Class hierarchy | ECS |
|---|---|---|
| A new kind of object | a new class, placed in the tree | a new combination of existing components |
| Behavior shared by two branches (a Bydo and a ship both fire) | moved up into a common base, which the others then carry, or a diamond | one component and one system |
| The server without rendering | `draw()` and its data must be split out of every class | it does not add the rendering systems |
| Finding what acts on what | in the class of the object | spread across systems, whose order must be stated in one place |
| Code to write first | none beyond the classes | entities, storage, iteration, scheduler |

Robert Nystrom describes the same limit in [Game Programming Patterns](https://gameprogrammingpatterns.com/component.html): he calls inheritance "too cumbersome for simple code reuse" and lists what components cost in return, "a good bit of complexity" and a "level of indirection" on every access.

Speed is not why this project picked an ECS. In the measure of [Components](#components), a hierarchy walks its objects in about 1 ns each and `forEach` takes 28 ns, because of how `forEach` finds its components. Neither figure matters at the scale of this game; the reasons above do.

## Components

A component is a plain struct attached to an entity by type: `ComponentRegistry` offers `add<T>`, `get<T>`, `has<T>` and `remove<T>`, and `destroy(entity)` removes every component of the entity before the entity is destroyed. A stale handle (an entity destroyed, its index recycled) reaches nothing: reads find no component, `remove` does nothing and `add` throws `DeadEntityException`.

Each component type has its own `ComponentStorage<T>`, created the first time the type is added. A storage keeps its components in one contiguous `std::vector<T>`, and an `EntityIndexMap` tells at which position the component of an entity index sits (a sparse set). Add, lookup and removal are constant time; a removal moves the last component into the hole, so the array never has gaps. The other side of the sparse set, from entity index to position, is still as long as the highest index in use (8 bytes per slot and per type), but nothing walks it.

Alternative considered: one `std::vector<std::optional<T>>` per type, indexed by entity index (a sparse array). It is simpler, but every type has a slot of its full size for each index up to the highest one in use, and a walk visits them all, even for a type few entities have. In a level where bullets come and go, it touches memory that holds nothing.

Alternative considered: archetypes, one table per exact combination of components, as Flecs, Unity DOTS and Bevy do. Entities of a table have every column, so a walk needs no test per entity, and it is the fastest of the measure below. But a table must exist for each combination, and adding or removing a component moves the entity and all its components to another table. Michele Caini, the author of EnTT, a sparse set library, compares both in [ECS back and forth](https://skypjack.github.io/2019-03-07-ecs-baf-part-2/) and concludes that the choice is "a matter of taste" more than performance. Here it comes down to the code: a sparse set is three arrays per type, and the walks of this game do not need more.

### Measured cost

A throwaway program ran the code of `rtype_engine` next to minimal versions of the alternatives, moving a `Position` by a `Velocity`. Before the walks, 2n entities are created and a random half destroyed, so the arrays hold what a level leaves behind. Median of 9 series, compiled with `-O2`, on an Apple M5 with Apple clang 21.0.0, on November 2, 2026; a second run gave the same figures within 2 ns.

| Walk, ns per entity | n = 100 | n = 1,000 | n = 10,000 |
|---|---|---|---|
| `ComponentRegistry::forEach`, today | 30.1 | 27.5 | 27.8 |
| The same sparse sets, walked without the type lookups | 1.8 | 1.6 | 1.7 |
| Sparse array | 0.8 | 1.2 | 4.9 |
| Archetype table | 0.1 | 0.1 | 0.2 |
| Class hierarchy, virtual `update()` | 0.7 | 0.5 | 1.0 |

| Add then remove one component, ns per entity | n = 100 | n = 1,000 | n = 10,000 |
|---|---|---|---|
| `ComponentRegistry`, today | 21.6 | 21.3 | 21.9 |
| Archetype, a row of 6 components moved twice | 16.2 | 17.6 | 52.6 |

These figures say three things:

- **The storage is not what costs most today.** For each entity, `forEach` looks up the storage of a type by hash three times, and the postponed destructions once. Walking the same sparse sets without them is about 17 times faster. This could change without touching the storage; nobody has done it because nothing asks for it yet.
- **It does not matter at this scale.** The only capture of a game holds 6 entities on average (see [Protocol](/R-Type/protocol/#world-state)), without missiles. Ten systems walking 1,000 entities each would take 280 µs, 1.7% of a tick of 16.7 ms.
- **The comparison has limits.** The alternatives are minimal: the archetype move does not look for its destination table, and the objects of the hierarchy are allocated in creation order, where a long game may scatter them. The sparse array gains from being half full; a type few entities have was not measured.

## Systems

A system is an object implementing `ISystem`; `SystemScheduler` calls them in the order they were added, so the order is decided in one place and is the same on every tick. `ComponentRegistry::forEach<First, Second>` gives a system the entities that have both components.

Destroying an entity inside a `forEach` is postponed until the outermost `forEach` returns, because removing a component moves the last one into the hole and would shift the walk. An entity destroyed this way is no longer visited. Adding or removing a component during a `forEach` is refused with an exception rather than allowed to corrupt the walk.

Alternative considered: a numeric priority on each system, sorted by the scheduler. It lets a system be placed without touching the others, but the order then lives in numbers scattered across classes and two systems can tie. A single list of `add()` calls is the one place that says what runs after what.

## Entity types

An entity type (a Drifter, the ship of a player, a missile) is a sheet of data, not a piece of C++: a name, the components an entity of that type starts with and their values, the pilot that steers it, and how it looks. The engine and the systems only know components, so a new enemy is a new sheet, plus a new behaviour when none fits, and never a change to a system or to the protocol. The subject of the Advanced Architecture track asks for an engine with proper abstractions and notes that entity behaviours are usually deferred to scripts; a named behaviour is the place where a script will plug in.

The pilot and the body exist: `PilotSystem` and the behaviours of `src/game/Behaviours/` steer the Bydos, and `MovementSystem` moves them by their `Intent`, as it moves the ships. The type sheet and the type table are a contract: the types are still written in C++ (`World::spawnPlayer`, `World::spawnBydo`, `BydoKind`) and fixed on the wire (`WireEntityType`), and the code must follow the rules below as it moves to sheets.

### Pilot and body

A body is an entity with an `Intent`, what it wants at this tick: a direction and whether it fires. A pilot writes the `Intent`; the systems that move and arm a body read the `Intent` only and never ask who wrote it. A pilot is either a player, whose input sets the `Intent` of its ship (`steerShipByInput()`, as today), or a behaviour: a named piece of code the server registers when it starts, which reads its own body and writes its `Intent` once per tick. A behaviour may read the `Position` of its body, how long the body has existed, the length of the tick and its own settings from the sheet; it writes nothing but that `Intent`. A sheet naming an unknown behaviour stops the server before it listens.

The direction of an `Intent` becomes two numbers, x and y, each from -1 to 1, instead of the four held directions of `ShipDirections`: a wave cannot be steered with four switches. A player writes -1, 0 or 1 on each axis, so a ship moves exactly as today. How a direction becomes movement is the motion of the body, declared by its sheet:

| Motion | Moves the body | Used by |
|---|---|---|
| `ship` | drift, roll and dash of `shipAfter()`, which reads the sign of each axis | the ships; the client runs the same `shipAfter()` to predict its own ship |
| `steady` | at its top speed times the direction, on each axis | the Bydos; the missiles still fly by their `Velocity` until they get a sheet |

A Weaver is a `steady` body with a top speed of 240 logical units per second (`BYDO_SPEED`): its behaviour writes x = -1 and picks y so the body ends the tick 48 logical units times sin(2π × age / 2 s) away from its row, the path the Weaver followed before the pilots. A path faster than the top speed on an axis is clamped to it. `PilotSystem` runs before every system that reads the `Intent`.

Alternative considered: keep paths that write the position, as the former `PatternSystem` did. It is exact and shorter, but a body then has two movers that ignore each other, the ships and the Bydos move by different rules, and a script could only replace a path, never steer.

Alternative considered: one system per kind of path. It needs no registry, but every new enemy would add a system and a line to the list of systems.

### The type sheet

Each type is a TOML file in `assets/entities/`, named after the type: lowercase letters, digits and underscores, `weaver.toml` for `weaver`. The table `[gameplay]` is read by the server only, `[appearance]` by the client only. The file is as strict as the [waves file](/R-Type/waves-file/): a missing or unknown key stops the program when it starts, a number is an integer, and a key with a unit says it in its name (`_ms`). The waves file names sheets, and a name with no sheet stops the server.

The keys are **provisional**: they are fixed when the loader is written, and code must read them through the loader's constants, never by spelling them again. `[gameplay]` holds the side of the body (`player`, `enemy`, `player_missile`, `enemy_missile`, which becomes its `EntityType`), its hitbox in logical units, its health or its lives, its points, its pilot (`player` or the name of a behaviour), its motion and top speed, and its gun if it has one: the interval between two shots in milliseconds and the type of its projectile. The settings of a behaviour sit in `[gameplay.behaviour]`; each behaviour declares the keys it reads, and they are checked when the server starts, so a misspelt setting stops the server instead of running with a default. `[appearance]` holds the sprite sheet, the idle animation and one animation per action.

Alternative considered: JSON. It is just as easy to read, but the project has no JSON library, it allows no comment to explain a value, and the waves file already sets the TOML rules.

Alternative considered: sheets written in Lua. A sheet could compute its values, but the server would need an interpreter before any behaviour needs one, and a sheet that runs code can no longer be checked before a game starts.

### Types on the network

A type is named by its sheet, never by a number in the code. The server numbers the types it loaded, from 1 to 255, and sends the table of names and numbers to a player once admitted, before any `EntitySpawned`; `EntitySpawned` then carries that number, still one byte. A number belongs to a session, so two changes that add a type never compete for a value, and a client that has no sheet for a name knows it on admission rather than in the middle of a game. The table is sent to an admitted session only, never in answer to a datagram from an unknown sender. Its wire layout, how its delivery is guaranteed and its size bounds are written on the [Protocol](/R-Type/protocol/) page with the message; the change needs a new protocol version.

Alternative considered: a hash of the name instead of a table. It saves the message, but two names can give the same hash, and the collision shows only the day it happens.

## Modules

A module is a feature a program can switch on, switch off or replace. The subject expects an engine to take no more space than needed, on disk and in memory, and gives modularization as a good way to get there: choosing what is compiled, linked or loaded at run time. So code with no life cycle, a system or a service is not a module; the [Modules](/R-Type/modules/) page gives the definition.

A program is a list of modules, objects implementing `IModule`, handed to an `Application`. The application installs them all, then starts them all, in list order, and stops them in the reverse order, so a module is stopped before its dependencies. The list is checked when the application is built: a module whose dependency is not listed before it is refused before anything is installed, so a missing module never leaves a socket or a thread half open. When a start fails, the modules already started are stopped before the error goes up. Stopping first stops the loop: the application waits for the pass in flight, so no work of a stage runs while the modules stop.

A program also declares the named stages of its loop, and a module adds its work to one of them while it installs; within a stage, the work runs in the order of the list. A module hands an object to the modules installed after it as a service found by type during install only, so what a module depends on shows in its constructors; a required service no earlier module provides stops the program before it starts, while an optional one is found to be absent.

Alternative considered: sorting the modules by their dependencies. A module could then be listed anywhere, but the order would be computed instead of read in one place, and two modules that do not depend on each other would have no defined order. As with systems, the list is the one place that says what starts after what.

Alternative considered: calling any folder with a CMake library a module. Nothing would need deciding, but the word would say nothing of what a program can leave out, and a folder called a module could hold nothing with a life cycle.

What a module does at each moment, and how to write one, is on the [Modules](/R-Type/modules/) page.

## Events

A system tells the rest of the game what happened through `EventBus` rather than by calling it: it publishes a typed event, and every callback subscribed to that type is called. A generic collision system in `rtype_engine` can therefore report a hit to the rules in `rtype_game`, which the engine may not link.

Delivery is deferred. `publish()` queues the event, `dispatch()` delivers the queue, and the loop must call `dispatch()` once per tick, after the systems. A system publishes from inside a `forEach`, where adding or removing a component is refused, so a subscriber called at that moment could not spawn an explosion; called after the systems, it can. Events published during `dispatch()` are delivered by the same `dispatch()`, so a collision, the damage it causes and the destruction that follows are all handled in the tick they happened.

Alternative considered: calling the subscribers inside `publish()`. It needs no queue and no `dispatch()` call, but every subscriber would run inside the publishing system's `forEach` and could not add or remove a component.

Alternative considered: a system calling the rules directly. It is the shortest code, but `rtype_engine` may not link `rtype_game`, and each new reaction to a collision (the score, the lives, the message to the players) would change the code that detects it.

A queue is what Robert Nystrom calls an [Event Queue](https://gameprogrammingpatterns.com/event-queue.html), whose intent is to "decouple when a message or event is sent from when it is processed". He lists its risks, and this bus answers each:

| Risk | Here |
|---|---|
| A queue is a global | each game has its own bus, handed to what uses it |
| The world changed between the send and the delivery | an event holds its data by value, and a stale `Entity` handle reaches nothing, because of its generation |
| Feedback loops | he advises against sending from code that handles an event; this bus allows it, so a chain ends in its tick. The cost: a subscriber that publishes the event it receives makes `dispatch()` run forever |
| The sender gets no answer | an event only says what happened; nothing in the game waits for a reply |

## Queues between threads

Threads must never share game state: a thread hands data to another through a `BoundedQueue` (`rtype_concurrency`), and the receiving thread drains it when it is ready, so the thread that runs the game never waits for a message from the network; it only waits while the network thread holds the queue's lock for one push. A queue has a fixed room: when it is full, the oldest message is discarded and counted. The newest input of a player matters more than an old one, and a client that floods the server cannot make the queue grow. A flood still pushes the other senders' messages out of a full queue, so unwanted datagrams must be filtered on the network thread, before the queue.

Alternative considered: a lock-free queue for one writer and one reader. It avoids the mutex, but its writer cannot discard the oldest message without moving the reader's position, which only the reader may do. A `push()` and a `drainInto()` take about 20 ns together when no other thread holds the lock (measured on an Apple Silicon laptop), so a few hundred messages per second cost a few microseconds per second.

Alternative considered: making the writer wait while the queue is full. Nothing is discarded by the queue, but the network thread stops receiving while the game is late, and the system then drops the datagrams that keep arriving, without counting them.

## Time

The simulation advances at a fixed rate (`SIMULATION_TICKS_PER_SECOND`, 60), whatever the rendering rate. `FixedTimestep` (`rtype_engine`) turns the time read from an `IClock` into a whole number of ticks to simulate; the remainder is kept for the next call, and a stall longer than `MAXIMUM_TICKS_PER_ADVANCE` ticks is dropped rather than caught up. Speeds are expressed in units per second and multiplied by the tick duration: `MovementSystem` (`rtype_game`) does it for every entity that has a `Position` and a `Velocity`. Tests drive a simulated clock, so the result is the same at 30, 60 and 144 frames per second.

A fixed step is what makes a simulation repeatable: in [Fix Your Timestep!](https://gafferongames.com/post/fix_your_timestep/), Glenn Fiedler notes that "the behavior of your physics simulation depends on the delta time you pass in". `FixedTimestep` is his accumulator, where "the renderer produces time and the simulation consumes it in discrete dt sized steps". The server has no renderer: its time comes from `TickLoop`, which sleeps until the next tick is due.

The limit of `MAXIMUM_TICKS_PER_ADVANCE` (5 ticks, about 83 ms) answers what Fiedler calls the spiral of death, where "being behind causes your update to simulate more steps to catch up, which causes you to fall further behind". After a longer stall the excess is dropped, so the game loses time rather than never catching up. His sample clamps a frame to 0.25 s; 5 ticks is a shorter limit, picked for this game. Fiedler also interpolates the rendering with the remainder of the accumulator. Nothing does so here, because the client runs no tick: its frame loop plays its animations with the real time of each frame, and only the server simulates at a fixed step.

Alternative considered: a variable step, one update per loop with the elapsed time. It needs no accumulator, but a collision or a wave then depends on how fast the machine runs, and a test must reproduce a frame rate to reproduce a bug.

`TimerScheduler` runs a callback after a delay or at a regular interval, and can cancel it through the `TimerHandle` returned when it is scheduled. It never reads a clock: the caller passes `SIMULATION_TICK_DURATION` to `advance()` once per tick returned by `FixedTimestep`, so timers follow simulation time and not wall-clock time. A repeating timer fires once per elapsed interval even when several pass in one `advance()`, timers due together fire in creation order, and a callback may schedule or cancel timers, itself included.

## Algorithms written by the team

Everything else comes from a library: the hash tables and the sort from the standard library, the random numbers from `std::mt19937`, the sockets from Asio. These are written here, each for the reason given.

| Algorithm | Where | Cost | Why this one | What it was weighed against |
|---|---|---|---|---|
| Sparse set with swap removal | `EntityIndexMap`, `ComponentStorage` | constant per operation | see [Components](#components) | sparse array, archetypes |
| Free list of indices with generations | `EntityRegistry` | constant per operation | an index is reused, so the arrays stay short, and a stale handle is told apart | never reusing an index: the arrays grow for as long as the game lasts |
| Accumulator with a limit | `FixedTimestep` | constant per call | see [Time](#time) | a variable step |
| Search of the earliest timer in a list | `TimerScheduler` | linear in the timers, per search | two places outside the engine schedule timers, both in `WaveDirector`, and the first of equal times in the list is the oldest, which gives creation order for free | a binary heap: logarithmic, but it needs a counter to keep that order, and cancelling stays linear |
| Queue delivered by a scan of the subscriptions | `EventBus` | linear in the subscriptions, per event | six subscriptions exist in the code; finding the next one by identifier after each call lets a subscriber unsubscribe during a delivery | one list per event type: faster with many subscribers |
| Ring buffer that discards the oldest | `BoundedQueue` | constant per `push()` | see [Queues between threads](#queues-between-threads) | a lock-free queue, a writer that waits |
| Test of every pair of boxes | `CollisionSystem`, `overlaps()` | quadratic in the boxes | see below | a grid or a sweep along one axis |
| Serial number comparison ([RFC 1982](https://www.rfc-editor.org/rfc/rfc1982)) | `isNewerSequence()` | constant | a 16-bit counter wraps during a game: newer means ahead by 1 to 32767, and 32768 is never newer | a plain `>`, wrong after the wrap; a wider counter, larger in every datagram |
| SipHash-2-4 and cookies kept by nobody | `sipHash()`, `CookieIssuer` | linear in the short message hashed | the cookies keep nothing before the sender proves its address, and whoever lacks the key cannot compute a cookie (the rate limit keeps one slot per address and port, in a table of fixed size); checked against the vectors of the reference implementation | a table of pending requests, which a forger fills; a cryptography library, a new dependency for one function |
| Generic cell rate algorithm, in a table of fixed size | `spendAllowance()`, `RequestAllowances` | constant per datagram, plus the SipHash of the address for a request | one time per sender, and a silence never gives back more than a burst; the table never grows, and a row picked by a keyed hash keeps forged senders from pushing a banned one out while that row holds a sender that is not banned; a row of banned senders gives up the ban that ends first | a window that counts the datagrams of each second: twice the rate can pass across the edge of two windows; a table that refuses new senders once full, as ioquake3 does: forged addresses then lock every new player out |
| Coordinates in eighths of a unit, states split across datagrams, states written as differences | `WorldStateCodec`, `DeltaWorldStateCodec` | linear in the entities | 16 bits instead of a 32-bit `float`, and every datagram stays within `MAX_DATAGRAM_SIZE` (1200 bytes) | see [Binary rather than text](#binary-rather-than-text) |
| Gaussian blur in two passes | `GaussianBlur` | 2 × (2r + 1) samples per pixel for a radius r | a Gaussian separates into one horizontal and one vertical pass, against (2r + 1)² samples in one pass | a single two-dimensional pass |

The cost of testing every pair grows with the square of the boxes. It was measured the same way as the storage, for 64 × 64 boxes spread over the play area and without gathering the boxes first:

| Boxes | Pairs | Time | Share of a tick of 16.7 ms |
|---|---|---|---|
| 100 | 4,950 | 2.6 µs | under 0.1% |
| 1,000 | 499,500 | 336 µs | 2% |
| 2,000 | 1,999,000 | 1.5 ms | 9% |

Nystrom's chapter on [spatial partition](https://gameprogrammingpatterns.com/spatial-partition.html) gives the same rule of thumb: pairwise tests grow "with the square of the number of units", yet "if your n is small enough, it may not be worth the bother", since a partition costs memory and must follow every object that moves. Here most boxes move on every tick. A grid becomes the answer if a profile of a real game shows this loop; several games sharing one simulation thread is what would bring that day closer.

## Coordinate system

Game logic uses one logical frame, the same on the server and on the client, and never a window size in pixels.

- The playfield is `PLAYFIELD_WIDTH` × `PLAYFIELD_HEIGHT` logical units, defined once in `rtype_game` (`src/game/Components/PlayfieldConstants.hpp`).
- These dimensions are 1920 × 1080: the playfield has the proportions of a 16:9 screen. Code must read the constants and never assume they match the window.
- The bottom `STATUS_BAND_HEIGHT` (96 logical units) of the playfield is covered by the status band every client lays over the game, so a match is played in the `PLAY_AREA_HEIGHT` (984) above it: whatever a player must see is placed there. The rows Bydos appear on keep half a Bydo and the room of a wave from its edges, so a Bydo stays in it even at the top and the bottom of a wave (see [Server](/R-Type/server/#bydos-bydospawner)).
- The origin is the top-left corner, x grows to the right and y grows downwards.
- `Position` is the center of an entity, `Velocity` is in units per second, `CollisionBox` is a size centered on the position.
- SFML places a sprite by its top-left corner by default, so the client must place each sprite so that its center falls on the position of its entity.
- The client is the only place that knows pixels. `GameWindow` gives the screens a render target whose view always covers the whole playfield, and `fitPlayfieldInWindow` (`src/client/Window/Display/`) picks the largest centered rectangle of the window with the proportions of the playfield; the rest stays black. Resizing the window changes that rectangle only, never a logical position or a speed.
- The window cannot be resized by dragging its border. Windowed, the player picks one of five sizes with the proportions of the playfield (`WINDOW_SIZES` in `src/client/Settings/Values/WindowConstants.hpp`: 960 × 540, 1280 × 720, 1600 × 900, 1920 × 1080, 2560 × 1440), so no black bar shows. `WindowSizeSelection` only offers the sizes strictly smaller than the desktop and opens the window at the largest of them. Borderless, the window covers the whole screen at the resolution of the desktop, which it never changes (see [Client](/R-Type/client/#window-gamewindow-windowlayout)). The black bars remain as a safety net for the cases where the system gives the window another shape, and fill the rest of a desktop without the proportions of the playfield.
- The display mode and the size are picked in the DISPLAY tab of the options, which SETTINGS opens on the title screen and Escape during a game: a list of the two display modes, then a list of the five sizes next to a chart of every size to scale, where a size too large for the desktop is dashed and can never be taken. While the window is borderless, the list of sizes is disabled and the chart is not drawn. Both are saved: the next launch opens the window in the mode picked, at the size picked when it still fits the desktop, at the largest size that fits otherwise (see [Client](/R-Type/client/#options-optionsscreen)). The fonts of the menus are loaded at launch through the asset library, which finds `assets/` whatever folder the client is launched from (see [Client](/R-Type/client/#assets-the-folder-and-the-ids)).

## Drawing order

The client draws a `Sprite` in its `Layer` (`src/game/Components/Layer.hpp`), from the first to the last: `Background`, `Enemies`, `Effects`, `Players`, `Popups`, `EnemyMissiles`, `Interface`. What can kill a player stays in sight: an explosion covers the enemies only, never a ship, the missiles of the players or the figure of a gain, and the missiles of the enemies are drawn over everything of the playfield. Effects made of loose pixels (debris, shockwaves, speed lines) are painted under every sprite.

Alternative considered: one layer for every entity, and the effects in front of it. It is simpler, and an explosion is never hidden, but the explosion of a Bydo then hides a missile the Bydo fired as it died for 11 frames, while that missile flies at a player.

## Network model

The game runs on one authoritative server: a client sends the keys it holds, the server runs every rule of the match and sends the state of the world back. Messages travel over UDP in a binary format (see [Protocol](/R-Type/protocol/)). The three choices below each have a cost, stated with them.

### UDP rather than TCP

TCP delivers every byte, in order. After a loss it holds back everything that follows until the missing segment is sent again, which takes at least one round trip. A game gains nothing from waiting: the state sent 50 ms later replaces the lost one, and an input is replaced 17 ms later by the next.

| | UDP | TCP |
|---|---|---|
| After a loss | the next datagram carries newer data and nothing waits | everything after the lost segment waits for it |
| Delivery | datagrams may be lost, duplicated or reordered | all bytes, in order |
| Header on top of IP | 8 bytes | 20 bytes at least |
| Connection | none: the protocol builds its own session | opened by the system |

The cost is that the protocol does the work TCP does. It drops a duplicate or a late datagram (`sequence`), checks who sends (`sessionToken`, and a cookie before a seat is given), and from version 2 sends the messages that matter until they are acknowledged (see [Guaranteed messages](/R-Type/protocol/#guaranteed-messages)).

Alternative considered: TCP for the lobby and UDP for the game. The lobby needs no speed, but it would take a second port and a second connection to open, and the lobby messages already survive losses by being repeated over the one UDP socket.

### Binary rather than text

A message is its fields written one after the other at a fixed size, so its size is known before it is read, and a receiver checks it with a comparison. Measured on the code of this repository, with the same fields written as compact JSON (the names of the protocol page, the 22 bytes of header included):

| Datagram | Binary | JSON |
|---|---|---|
| `PlayerInput` (60 per second) | 30 bytes | 196 bytes |
| `WorldState`, 2 players and 6 entities (20 per second) | 113 bytes | 722 bytes |

The binary `PlayerInput` is the output of `DatagramCodec::encode`. The binary `WorldState` is the average of the capture described in [Protocol](/R-Type/protocol/#world-state), 105 bytes, plus the 4 bytes per player of `acknowledgedInput` that were added after it (2 × 4 = 8, not captured again). Its JSON is a constructed example, not a capture, and was not rewritten with that field. That is about 1.8 kB per second against 11.8 kB for the inputs of one player, and 2.3 kB against 14.4 kB for the states sent to one: between six and seven times less.

The cost is that nobody reads a capture without the specification. The protocol page therefore gives every field byte by byte, in big-endian order so a number reads in a capture the way it is written, and a new field changes the version.

Alternative considered: JSON. A capture and a log read themselves, and a field is added without a layout. It loses on size, as above, on the parsing and the allocations every datagram would cost at 60 per second and per player, and on the bounds: a text length is known only after reading the text.

### An authoritative server rather than peer to peer

The client sends keys, never positions. The server moves the ships, fires, collides and scores (`rtype_game`), so there is one truth: two players never disagree about who was hit, and a modified client cannot move out of the rules or claim a hit, because the server decides from the keys alone and checks every datagram it receives. Only the server needs a reachable address. With 4 players, 4 streams go to the server and 4 come back; a mesh of peers would carry 12.

The cost is that a player sees the effect of a key after a round trip to the server, and that the game ends for everyone when the server stops. Hiding that round trip is what client-side prediction does (see [Project layout](/R-Type/project-layout/)).

Alternative considered: peer to peer, with every peer running the game. It needs no server to host, but the peers must compute exactly the same result, every pair must reach the other through its router, and any peer can cheat. A peer acting as host would be a server whose owner also has an advantage in the game.

### Known limits of the proof of concept

- **Losses.** State messages repeat what they say, so they correct themselves. `EntityDestroyed`, `GameEvent` and `PlayerLeft` are guaranteed: sent again every 116.7 ms until acknowledged, for 1 s at most. With 20 % of datagrams lost, about one in two million is still lost; a sound or an effect is then missed, and lives, scores and players stay right through the states (see [Guaranteed messages](/R-Type/protocol/#guaranteed-messages)).
- **Latency.** The client predicts its own ship (PREDICTION in the options, on by default), so a key shows at once, and shows the other entities in the past, between two states, so they move smoothly even when a state is lost: two state intervals behind the newest state, which is 100 ms at 20 states per second and 200 ms at 10, and more for a while after a state arrived late (see [Client](/R-Type/client/#other-entities-interpolation-interpolationclock-positionhistory)). The events a state does not carry (an explosion, a sound) are not delayed with them. With PREDICTION off, a key shows after a round trip plus up to one state interval.
- **Measures.** The rates and delays are **provisional**. The only capture is a local game, with no loss and no latency. The network simulator now adds them on demand (see [Network simulator](/R-Type/network/#network-simulator-simulatednetworksocket)), and so can the system, with the scripts of `tests/tools/network-conditions/` (see [Network conditions from the system](/R-Type/network/#network-conditions-from-the-system-teststoolsnetwork-conditions)), but no measurement of the game has been taken with either yet.

The second part answers each point (see [Version 2](/R-Type/protocol/#version-2)): guaranteed messages replaced the three copies in version 2; an input is to be repeated until the server confirms it, and states are to be sent as differences. The state already confirms the newest input of each player, which is what lets the client replay its newest inputs on top of a state and correct its ship. Prediction and interpolation themselves are client work, placed in `client/Prediction` (see [Project layout](/R-Type/project-layout/)).

## Master

The master lists the game servers and directs them: each server tells it the games it hosts, a player reads that list, an admin sends orders. It never runs a game and nothing of a game goes through it, so a game never waits for it and goes on when it stops. Its code is described in [Master](/R-Type/master/).

### HTTP between the programs and the master

The subject requires UDP between the server and the clients, accepts a second connection over TCP for specific cases with a strong justification, and requires every in-game message (entities, movements, events) to travel over UDP. The master carries no in-game message: a list of servers, a choice of server, the news a server sends every few seconds and the orders of an admin. It answers them over HTTP, which runs over TCP, for four reasons:

- **The browser.** The subject accepts a web administration, and a browser speaks HTTP, never raw UDP or TCP: the master needs HTTP for the web site whatever the game uses.
- **One protocol, not two.** HTTP already frames a message, names its route and its status, and carries the errors; a protocol of our own over TCP or UDP would write all of it again, beside the HTTP the browser needs.
- **The tools.** A request reads with `curl`, and a reverse proxy adds HTTPS in front of the master without a line of code in it.
- **Nothing depends on it to play.** A player still joins a server by its address, over UDP, when the master is stopped.

The game protocol stays binary, as the subject requires for the communications between the client and the server; the API of the master answers JSON, the text a browser reads, for messages a few times a minute. DDNet registers its servers the same way, with an HTTP request to its master server.

Alternative considered: UDP with messages of our own. It keeps one transport for everything, but the web site would still need HTTP, so the master would speak two protocols, and the messages would have to be made reliable again over UDP.

### A mode of the server binary

The master is `r-type_server --mode master`. The subject lists two binaries, `r-type_server` and `r-type_client`: the master ships inside the first, with nothing more to build or deliver. It reuses what the server already has, the stop on Ctrl-C and SIGTERM, the configuration file and the journal, so `main()` runs either program through one function. A failure stays apart all the same, since the master and a server are two processes: the master that crashes stops no game.

The cost is that the server binary holds the code of the master, cpp-httplib and nlohmann-json included, even where it only hosts games. `rtype_forbid_reachable_links` keeps the master away from SFML, Asio, the game and the code of the server, through every link it reaches.

Alternative considered: a third binary, `r-type_master`. It keeps the server smaller and the master deployable alone, but it is a binary the subject does not list, one more to build, package and ship, and it would have needed the stop and the launch of the server moved into the engine first.

### Keys and cryptography: libhydrogen

A game server proves who it is with the key the master gave when its host created it (`POST /api/servers`), as a FiveM server is known by its license key, sent as `Authorization: Bearer <key>` (RFC 6750) and never named by an id in the path, so a key reaches its own server alone. Each server has a key of its own, so no host can act as another one. The key is 256 bits from the random source of the system, shown once, and the master keeps only its hash, so a copy of its database registers nobody. Both come from libhydrogen, which the accounts and the signed tickets will use too.

The subject asks that dependencies go through the package manager and that the project build without altering the system. libhydrogen, by the author of libsodium, builds with CMake through vcpkg and needs nothing installed. Its password hash is not Argon2id: it costs computation and no memory, and a master key, outside the database, encrypts every stored hash, so a stolen database cannot be used to test passwords.

Alternative considered: libsodium. It is the reference, with Argon2id for passwords, but its vcpkg port builds with autotools on macOS and Linux, which every developer and the CI would have to install on the machine, against the subject.

## Consequences

- `EntityRegistry`, `ComponentRegistry`, `ISystem`, `SystemScheduler`, `EventBus` and `BoundedQueue` exist today. The game screen of the client calls a scheduler once per frame to play its animations; the tick of the server runs the stages of its `Application`: the games move on and run their systems and their timers, the connection requests are answered, and the events of the bus are delivered.
- Any new target declares its links explicitly; a link that breaks the table above fails the configure step.

## Validation

| Team | Status |
|---|---|
| Team 1 | pending |
| Team 2 | pending |
| Team 3 | pending |
| Team 4 | pending |
