---
title: 'Architecture decision: engine, game and network libraries'
sidebar:
  label: Architecture
---

## Context

The client renders with SFML, the server talks over the network with Asio, and both must run the same game logic. If rendering, networking and logic live in one place, no part can be changed or tested alone, and the server would be forced to link a graphics library.

## Decision

The code is split into three static libraries with one-way dependencies, and the game objects are organised as an **ECS** (Entity Component System).

```
r-type_client -> rtype_client_game_window -> SFML
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
- Data is separate from behaviour, which matches the engine/game split: the engine knows how to store and iterate, the game decides what a component or a system means.
- Client and server share the same components; only the systems differ (the client adds rendering and input, the server adds authority).
- A new enemy or power-up is a new combination of components, not a new class in a hierarchy.

Alternative considered: a classic class hierarchy of game objects with virtual `update()` and `draw()`. It is simpler at first, but ties logic to rendering, which the server cannot afford, and makes cross-cutting behaviours (a boss that is also a shooter) awkward.

## Components

A component is a plain struct attached to an entity by type: `ComponentRegistry` offers `add<T>`, `get<T>`, `has<T>` and `remove<T>`, and `destroy(entity)` removes every component of the entity before the entity is destroyed. A stale handle (an entity destroyed, its index recycled) reaches nothing: reads find no component, `remove` does nothing and `add` throws `DeadEntityException`.

Each component type has its own `ComponentStorage<T>`, created the first time the type is added. A storage keeps its components in one contiguous `std::vector<T>`, and an `EntityIndexMap` tells at which position the component of an entity index sits (a sparse set). Add, lookup and removal are constant time; a removal moves the last component into the hole, so the array never has gaps and a system that walks one type reads one compact array.

Alternative considered: one `std::vector<std::optional<T>>` per type, indexed by entity index. It is simpler, but the array has as many slots as the highest index in use, most of them empty in a level where bullets come and go, so walking one type touches memory that holds nothing.

## Time

The simulation advances at a fixed rate (`SIMULATION_TICKS_PER_SECOND`, 60), whatever the rendering rate. `FixedTimestep` (`rtype_engine`) turns the time read from an `IClock` into a whole number of ticks to simulate; the remainder is kept for the next call, and a stall longer than `MAXIMUM_TICKS_PER_ADVANCE` ticks is dropped rather than caught up. Speeds are expressed in units per second and multiplied by the tick duration. Tests drive a simulated clock, so the result is the same at 30, 60 and 144 frames per second.

`TimerScheduler` runs a callback after a delay or at a regular interval, and can cancel it through the `TimerHandle` returned when it is scheduled. It never reads a clock: the caller passes `SIMULATION_TICK_DURATION` to `advance()` once per tick returned by `FixedTimestep`, so timers follow simulation time and not wall-clock time. A repeating timer fires once per elapsed interval even when several pass in one `advance()`, timers due together fire in creation order, and a callback may schedule or cancel timers, itself included.

## Coordinate system

Game logic uses one logical frame, the same on the server and on the client, and never a window size in pixels.

- The playfield is `PLAYFIELD_WIDTH` × `PLAYFIELD_HEIGHT` logical units, defined once in `rtype_game` (`PlayfieldConstants.hpp`).
- These dimensions are 1920 × 1080: the playfield has the proportions of a 16:9 screen. Code must read the constants and never assume they match the window.
- The origin is the top-left corner, x grows to the right and y grows downwards.
- `Position` is the center of an entity, `Velocity` is in units per second, `CollisionBox` is a size centered on the position.
- SFML puts the origin of a sprite at its top-left corner by default, so the client sets it to the center of each sprite.
- The client is the only place that knows pixels: it scales the playfield to its window.

## Consequences

- `EntityRegistry` and `ComponentRegistry` exist today; systems are added by the next stories.
- Any new target declares its links explicitly; a link that breaks the table above fails the configure step.

## Validation

| Team | Status |
|---|---|
| Team 1 | pending |
| Team 2 | pending |
| Team 3 | pending |
| Team 4 | pending |
