---
title: Engine
description: What the engine does, how its parts fit together and where to find them in the code.
---

The engine is the generic part of the game: it stores game objects, advances time at a fixed rate and runs delayed work. It knows nothing about R-Type, SFML or sockets. The rules of the game live in `rtype_game`, which is built on top of it (see [Architecture](/R-Type/architecture/)).

This page tells you where to look when you need to change something. It describes what exists today and says plainly what does not exist yet.

| Part | State | Where |
|---|---|---|
| Entities | Available | `src/engine/EntityRegistry/`, `src/engine/Entity.hpp` |
| Components | Available | `src/engine/Components/` |
| Fixed timestep and timers | Available | `src/engine/Time/` |
| Systems | Not yet written | — |
| Game loop | Not yet written: the pieces exist, nothing assembles them | — |
| Event bus | Not yet written | — |

## How the parts fit together

```mermaid
flowchart LR
  Clock[IClock] --> Step[FixedTimestep]
  Step -- "ticks to simulate" --> Loop["game loop (to write)"]
  Loop -- "once per tick" --> Timers[TimerScheduler]
  Loop -- "once per tick" --> Systems["systems (to write)"]
  Systems --> Components[ComponentRegistry]
  Components --> Entities[EntityRegistry]
```

The client and the server will each own one loop. The engine provides the building blocks; the loop and the systems are added by the next stories.

## ECS: entities and components

An **entity** is only a handle, `Entity { index, generation }` (`Entity.hpp`). It carries no data. A **component** is a plain struct attached to an entity by type, for example `Position`, `Velocity` and `CollisionBox` in `src/game/Components/`.

### Entities: `EntityRegistry`

`EntityRegistry` creates and destroys entities and recycles the index of a destroyed one. The generation tells apart the successive entities that lived in the same slot, so a handle kept after a destruction never matches the entity that took its place: `isAlive()` is false for it.

### Components: `ComponentRegistry`

`ComponentRegistry` owns one `ComponentStorage<T>` per component type and offers `add<T>`, `get<T>`, `has<T>` and `remove<T>`.

- `get<T>` returns a pointer, or `nullptr` when the component is absent or the entity is dead. The pointer is invalidated by a later `add` or `remove` of the same type.
- `add<T>` throws `DeadEntityException` on a dead entity and replaces a component of the same type.
- Entities must be destroyed through `ComponentRegistry::destroy`, never through `EntityRegistry` directly, otherwise their components stay behind.

Each `ComponentStorage<T>` keeps its components in one contiguous `std::vector<T>`, and an `EntityIndexMap` says where the component of a given entity index sits (a sparse set). Add, lookup and removal are constant time and the array has no gaps. The reasoning is in [Architecture](/R-Type/architecture/#components).

```cpp
rtype::engine::EntityRegistry entities;
rtype::engine::ComponentRegistry components(entities);

rtype::engine::Entity ship = entities.create();
components.add(ship, rtype::game::Position{.x = 100.0F, .y = 200.0F});

if (auto* position = components.get<rtype::game::Position>(ship)) {
  position->x += 1.0F;
}

components.destroy(ship);
```

## Game loop: time

The simulation runs at a fixed rate, `SIMULATION_TICKS_PER_SECOND` (60), whatever the rendering rate (`TimeConstants.hpp`).

- `IClock` is the source of time. `SystemClock` reads the real clock; tests use `tests/SimulatedClock.hpp`, so a test never waits and gives the same result at any frame rate.
- `FixedTimestep` turns the time read from an `IClock` into a whole number of ticks to simulate. The remainder is kept for the next call. A stall longer than `MAXIMUM_TICKS_PER_ADVANCE` ticks is dropped instead of being caught up.
- `TimerScheduler` runs a callback after a delay (`scheduleOnce`) or at a regular interval (`scheduleRepeating`), and cancels it through the `TimerHandle` it returned. It never reads a clock: the caller passes `SIMULATION_TICK_DURATION` to `advance()` once per tick.

The loop the client and the server will write looks like this:

```cpp
const std::size_t ticks = timestep.consumeTicks();
for (std::size_t tick = 0; tick < ticks; ++tick) {
  timers.advance(rtype::engine::SIMULATION_TICK_DURATION);
  // systems run here, once their story is done
}
```

The loop itself does not exist yet: today only the pieces above are in `rtype_engine`, and `GameWindow` (`src/client/Window/GameWindow/`) does not use them.

## Event bus

There is no event bus in the code yet. This page will describe it, its folder and its classes when the story that adds it is merged. Until then, nothing in the engine publishes or subscribes to events.

## Where to intervene

| You want to… | Look at |
|---|---|
| Add a component type | A new struct in `src/game/Components/`; nothing to register, `ComponentRegistry` creates its storage on first use |
| Change how components are stored | `src/engine/Components/ComponentStorage.hpp`, `EntityIndexMap/` |
| Change how entities are created or recycled | `src/engine/EntityRegistry/` |
| Change the simulation rate | `SIMULATION_TICKS_PER_SECOND` in `src/engine/Time/TimeConstants.hpp` |
| Run something after a delay | `TimerScheduler` in `src/engine/Time/TimerScheduler/` |
| Add an engine error | `src/engine/Exceptions/EngineException.hpp` |
| Write a test that depends on time | `tests/SimulatedClock.hpp`, `tests/FixedTimestepTest.cpp` |

Each folder has its own `CMakeLists.txt` declaring one library, linked into `rtype_engine` by `src/engine/CMakeLists.txt`.
