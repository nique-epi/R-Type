---
title: 'Architecture decision: engine, game and network libraries'
sidebar:
  label: Architecture
---

## Context

The client renders with SFML, the server talks over the network with Asio, and both must run the same game logic. If rendering, networking and logic live in one place, no part can be changed or tested alone, and the server would be forced to link a graphics library.

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

- A level holds many entities of a few kinds (bullets, enemies, players); an ECS stores their data contiguously and iterates over it cheaply.
- Data is separate from behavior, which matches the engine/game split: the engine knows how to store and iterate, the game decides what a component or a system means.
- Client and server share the same components; only the systems differ (the client adds rendering and input, the server adds authority).
- A new enemy or power-up is a new combination of components, not a new class in a hierarchy.

Alternative considered: a classic class hierarchy of game objects with virtual `update()` and `draw()`. It is simpler at first, but ties logic to rendering, which the server cannot afford, and makes cross-cutting behaviors (a boss that is also a shooter) awkward.

## Components

A component is a plain struct attached to an entity by type: `ComponentRegistry` offers `add<T>`, `get<T>`, `has<T>` and `remove<T>`, and `destroy(entity)` removes every component of the entity before the entity is destroyed. A stale handle (an entity destroyed, its index recycled) reaches nothing: reads find no component, `remove` does nothing and `add` throws `DeadEntityException`.

Each component type has its own `ComponentStorage<T>`, created the first time the type is added. A storage keeps its components in one contiguous `std::vector<T>`, and an `EntityIndexMap` tells at which position the component of an entity index sits (a sparse set). Add, lookup and removal are constant time; a removal moves the last component into the hole, so the array never has gaps and a system that walks one type reads one compact array.

Alternative considered: one `std::vector<std::optional<T>>` per type, indexed by entity index. It is simpler, but the array has as many slots as the highest index in use, most of them empty in a level where bullets come and go, so walking one type touches memory that holds nothing.

## Systems

A system is an object implementing `ISystem`; `SystemScheduler` calls them in the order they were added, so the order is decided in one place and is the same on every tick. `ComponentRegistry::forEach<First, Second>` gives a system the entities that have both components.

Destroying an entity inside a `forEach` is postponed until the outermost `forEach` returns, because removing a component moves the last one into the hole and would shift the walk. An entity destroyed this way is no longer visited. Adding or removing a component during a `forEach` is refused with an exception rather than allowed to corrupt the walk.

Alternative considered: a numeric priority on each system, sorted by the scheduler. It lets a system be placed without touching the others, but the order then lives in numbers scattered across classes and two systems can tie. A single list of `add()` calls is the one place that says what runs after what.

## Modules

A program is a list of modules, objects implementing `IModule`, handed to an `Application`. The application installs them all, then starts them all, in list order, and stops them in the reverse order, so a module is stopped before its dependencies. The list is checked when the application is built: a module whose dependency is not listed before it is refused before anything is installed, so a missing module never leaves a socket or a thread half open. When a start fails, the modules already started are stopped before the error goes up. Stopping first stops the loop: the application waits for the pass in flight, so no work of a stage runs while the modules stop.

A program also declares the named stages of its loop, and a module adds its work to one of them while it installs. That is meant to replace, over time, the three ways work is wired today: systems, `onTick` or `advance` calls written by hand, and bus subscribers. A module hands an object to the modules installed after it as a service found by type during install only, so what a module depends on shows in its constructors; a required service no earlier module provides stops the program before it starts, while an optional one is found to be absent.

Alternative considered: sorting the modules by their dependencies. A module could then be listed anywhere, but the order would be computed instead of read in one place, and two modules that do not depend on each other would have no defined order. As with systems, the list is the one place that says what starts after what.

## Events

A system tells the rest of the game what happened through `EventBus` rather than by calling it: it publishes a typed event, and every callback subscribed to that type is called. A generic collision system in `rtype_engine` can therefore report a hit to the rules in `rtype_game`, which the engine may not link.

Delivery is deferred. `publish()` queues the event, `dispatch()` delivers the queue, and the loop must call `dispatch()` once per tick, after the systems. A system publishes from inside a `forEach`, where adding or removing a component is refused, so a subscriber called at that moment could not spawn an explosion; called after the systems, it can. Events published during `dispatch()` are delivered by the same `dispatch()`, so a collision, the damage it causes and the destruction that follows are all handled in the tick they happened.

Alternative considered: calling the subscribers inside `publish()`. It needs no queue and no `dispatch()` call, but every subscriber would run inside the publishing system's `forEach` and could not add or remove a component.

## Queues between threads

Threads must never share game state: a thread hands data to another through a `BoundedQueue` (`rtype_concurrency`), and the receiving thread drains it when it is ready, so the thread that runs the game never waits for a message from the network; it only waits while the network thread holds the queue's lock for one push. A queue has a fixed room: when it is full, the oldest message is discarded and counted. The newest input of a player matters more than an old one, and a client that floods the server cannot make the queue grow. A flood still pushes the other senders' messages out of a full queue, so unwanted datagrams must be filtered on the network thread, before the queue.

Alternative considered: a lock-free queue for one writer and one reader. It avoids the mutex, but its writer cannot discard the oldest message without moving the reader's position, which only the reader may do. A `push()` and a `drainInto()` take about 20 ns together when no other thread holds the lock (measured on an Apple Silicon laptop), so a few hundred messages per second cost a few microseconds per second.

Alternative considered: making the writer wait while the queue is full. Nothing is discarded by the queue, but the network thread stops receiving while the game is late, and the system then drops the datagrams that keep arriving, without counting them.

## Time

The simulation advances at a fixed rate (`SIMULATION_TICKS_PER_SECOND`, 60), whatever the rendering rate. `FixedTimestep` (`rtype_engine`) turns the time read from an `IClock` into a whole number of ticks to simulate; the remainder is kept for the next call, and a stall longer than `MAXIMUM_TICKS_PER_ADVANCE` ticks is dropped rather than caught up. Speeds are expressed in units per second and multiplied by the tick duration: `MovementSystem` (`rtype_game`) does it for every entity that has a `Position` and a `Velocity`. Tests drive a simulated clock, so the result is the same at 30, 60 and 144 frames per second.

`TimerScheduler` runs a callback after a delay or at a regular interval, and can cancel it through the `TimerHandle` returned when it is scheduled. It never reads a clock: the caller passes `SIMULATION_TICK_DURATION` to `advance()` once per tick returned by `FixedTimestep`, so timers follow simulation time and not wall-clock time. A repeating timer fires once per elapsed interval even when several pass in one `advance()`, timers due together fire in creation order, and a callback may schedule or cancel timers, itself included.

## Coordinate system

Game logic uses one logical frame, the same on the server and on the client, and never a window size in pixels.

- The playfield is `PLAYFIELD_WIDTH` × `PLAYFIELD_HEIGHT` logical units, defined once in `rtype_game` (`src/game/Components/PlayfieldConstants.hpp`).
- These dimensions are 1920 × 1080: the playfield has the proportions of a 16:9 screen. Code must read the constants and never assume they match the window.
- The bottom `STATUS_BAND_HEIGHT` (96 logical units) of the playfield is covered by the status band every client lays over the game, so a match is played in the `PLAY_AREA_HEIGHT` (984) above it: whatever a player must see is placed there. The rows Bydos appear on keep half a Bydo and the room of a wave from its edges, so a Bydo stays in it even at the top and the bottom of a wave (see [Server](/R-Type/server/#bydos-bydospawner)).
- The origin is the top-left corner, x grows to the right and y grows downwards.
- `Position` is the center of an entity, `Velocity` is in units per second, `CollisionBox` is a size centered on the position.
- SFML places a sprite by its top-left corner by default, so the client must place each sprite so that its center falls on the position of its entity.
- The client is the only place that knows pixels. `GameWindow` gives the screens a render target whose view always covers the whole playfield, and `fitPlayfieldInWindow` (`src/client/Window/PlayfieldViewport/`) picks the largest centered rectangle of the window with the proportions of the playfield; the rest stays black. Resizing the window changes that rectangle only, never a logical position or a speed.
- The window cannot be resized by dragging its border. Windowed, the player picks one of five sizes with the proportions of the playfield (`WINDOW_SIZES` in `src/client/Window/WindowConstants.hpp`: 960 × 540, 1280 × 720, 1600 × 900, 1920 × 1080, 2560 × 1440), so no black bar shows. `WindowSizeSelection` only offers the sizes strictly smaller than the desktop and opens the window at the largest of them. Borderless, the window covers the whole screen at the resolution of the desktop, which it never changes (see [Client](/R-Type/client/#window-gamewindow-windowlayout)). The black bars remain as a safety net for the cases where the system gives the window another shape, and fill the rest of a desktop without the proportions of the playfield.
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

The cost is that the protocol does the work TCP does. Version 1 already drops a duplicate or a late datagram (`sequence`), checks who sends (`sessionToken`, and a cookie before a seat is given) and repeats the messages that matter. Guaranteed delivery is the work of the second part.

Alternative considered: TCP for the lobby and UDP for the game. The lobby needs no speed, but it would take a second port and a second connection to open, and the lobby messages already survive losses by being repeated over the one UDP socket.

### Binary rather than text

A message is its fields written one after the other at a fixed size, so its size is known before it is read, and a receiver checks it with a comparison. Measured on the code of this repository, with the same fields written as compact JSON (the names of the protocol page, the 22 bytes of header included):

| Datagram | Binary | JSON |
|---|---|---|
| `PlayerInput` (60 per second) | 30 bytes | 196 bytes |
| `WorldState`, 2 players and 6 entities (20 per second) | 105 bytes | 722 bytes |

The binary `PlayerInput` is the output of `DatagramCodec::encode`. The binary `WorldState` is the average of the capture described in [Protocol](/R-Type/protocol/#world-state), and its JSON is a constructed example, not a capture. That is about 1.8 kB per second against 11.8 kB for the inputs of one player, and 2.1 kB against 14.4 kB for the states sent to one: roughly seven times less.

The cost is that nobody reads a capture without the specification. The protocol page therefore gives every field byte by byte, in big-endian order so a number reads in a capture the way it is written, and a new field changes the version.

Alternative considered: JSON. A capture and a log read themselves, and a field is added without a layout. It loses on size, as above, on the parsing and the allocations every datagram would cost at 60 per second and per player, and on the bounds: a text length is known only after reading the text.

### An authoritative server rather than peer to peer

The client sends keys, never positions. The server moves the ships, fires, collides and scores (`rtype_game`), so there is one truth: two players never disagree about who was hit, and a modified client cannot move out of the rules or claim a hit, because the server decides from the keys alone and checks every datagram it receives. Only the server needs a reachable address. With 4 players, 4 streams go to the server and 4 come back; a mesh of peers would carry 12.

The cost is that a player sees the effect of a key after a round trip to the server, and that the game ends for everyone when the server stops. Hiding that round trip is what client-side prediction does (see [Project layout](/R-Type/project-layout/)).

Alternative considered: peer to peer, with every peer running the game. It needs no server to host, but the peers must compute exactly the same result, every pair must reach the other through its router, and any peer can cheat. A peer acting as host would be a server whose owner also has an advantage in the game.

### Known limits of the proof of concept

- **Losses.** Nothing is guaranteed in version 1. State messages repeat what they say, so they correct themselves; the messages that show something once are sent three times, 50 ms apart. If all three copies are lost, a sound or an effect is missed, and lives, scores and players stay right through the states (see [Losses](/R-Type/protocol/#losses)).
- **Latency.** The client neither predicts its own ship nor interpolates the others: it draws what the last state says, so a key shows after a round trip plus up to one state interval (50 ms at 20 states per second), and a ship moves at the pace of the states.
- **Measures.** The rates and delays are **provisional**. The only capture is a local game, with no loss and no latency. The network simulator now adds them on demand (see [Network simulator](/R-Type/network/#network-simulator-simulatednetworksocket)), but no measurement has been taken with it yet.

The second part answers each point (see [Version 2](/R-Type/protocol/#version-2)): guaranteed messages replace the three copies, an input is repeated until the server confirms it, and the state confirms the last input applied, which is what lets the client replay its newest inputs on top of a state and correct its ship. Prediction and interpolation themselves are client work, placed in `client/Prediction` (see [Project layout](/R-Type/project-layout/)).

## Consequences

- `EntityRegistry`, `ComponentRegistry`, `ISystem`, `SystemScheduler`, `EventBus` and `BoundedQueue` exist today. The game screen of the client calls a scheduler once per frame to play its animations; the tick of the server runs the stages of its `Application`: the games move on and run their systems, the connection requests are answered, the timers advance and the events of the bus are delivered.
- Any new target declares its links explicitly; a link that breaks the table above fails the configure step.

## Validation

| Team | Status |
|---|---|
| Team 1 | pending |
| Team 2 | pending |
| Team 3 | pending |
| Team 4 | pending |
