---
title: Server and game
description: What the game library does for a match, how its parts fit together and where to find them in the code.
---

The game library, `rtype_game`, holds the rules of a match: what moves, what touches what and what that means. It is built on the engine (see [Engine](/R-Type/engine/)), and it knows nothing about sockets, SFML or the messages sent between the programs (see [Project layout](/R-Type/project-layout/#game)). The server runs all of it; the client will only run the part that predicts its own ship.

This page tells you where to look when you need to change something. It describes what exists today and says plainly what does not exist yet.

| Part | State | Where |
|---|---|---|
| Components (`Position`, `Velocity`, `CollisionBox`, `EntityType`, `Sprite`, `Layer`) | Available | `src/game/Components/` |
| Movement | Available | `src/game/Systems/MovementSystem/` |
| Collisions | Available: nothing runs them yet | `src/game/Systems/CollisionSystem/`, `src/game/Systems/CollisionRules/`, `src/game/Events/` |
| Damage and destruction | Not yet written | — |
| Spawning, waves, score, lives | Not yet written | — |

## Collisions

A collision takes two steps, in two classes. The first finds what overlaps; the second decides which overlaps matter. Neither removes health nor destroys an entity: they only announce what happened, so the damage rules can react to it (see [Event bus](/R-Type/engine/#event-bus-eventbus)).

### Detection: `CollisionSystem`

`CollisionSystem` (`src/game/Systems/CollisionSystem/`, an `ISystem`) looks at every entity that has a `Position` and a `CollisionBox`. A `CollisionBox` is a size in logical units, centered on the `Position`. For each pair of boxes that overlap, it publishes one `Collision` (the engine event, see [Collisions](/R-Type/engine/#collisions-bounds-overlaps-and-collision)).

- Each overlapping pair is published once per update. Which of the two entities comes first carries no meaning.
- Boxes that only touch along an edge or at a corner do not overlap.
- An entity without a `Position` or without a `CollisionBox` is never reported.
- Every pair is tested, so the cost grows with the square of the number of boxes. The list of boxes is kept between updates, so a steady state allocates nothing.
- `update()` only queues the events. The loop must call `EventBus::dispatch()` after the systems, and run `CollisionSystem` after `MovementSystem`, so the boxes are tested where the entities now are.

### Rules: `CollisionRules`

`CollisionRules` (`src/game/Systems/CollisionRules/`) subscribes to `Collision` when it is created and unsubscribes when it is destroyed. For each `Collision` it reads the `EntityType` of both entities, in whichever order they come, and publishes a game event for three pairs only:

| Pair | Published |
|---|---|
| `PlayerMissile` and `Enemy` | `MissileHit{missile, target}`, the target being the enemy |
| `EnemyMissile` and `Player` | `MissileHit{missile, target}`, the target being the player |
| `Player` and `Enemy` | `ShipContact{player, enemy}` |

Every other pair is ignored: a player missile does not hurt a player, two enemies do not hurt each other, two missiles do not stop each other. An entity without an `EntityType`, or destroyed between the moment the `Collision` was published and the moment it is delivered, is ignored too.

The events are structs in `src/game/Events/`. `MissileHit` and `ShipContact` say that a contact happened; what it costs is left to the damage rules, which do not exist yet.

```cpp
rtype::engine::EventBus events;
rtype::game::CollisionRules rules{events, components};

scheduler.add(std::make_unique<rtype::game::MovementSystem>());
scheduler.add(std::make_unique<rtype::game::CollisionSystem>(events));

events.subscribe<rtype::game::MissileHit>([](const rtype::game::MissileHit& hit) {
  // remove health from hit.target, destroy hit.missile
});

scheduler.run(components, SIMULATION_TICK_DURATION);
events.dispatch();
```

The bus and the registry must outlive `CollisionRules`, and the bus must outlive `CollisionSystem`. The game loop that would create them does not exist yet: today only the tests build this chain.

## Where to intervene

| You want to… | Look at |
|---|---|
| Change which pairs of entities matter | `CollisionRules::publishIfRuleMatches` in `src/game/Systems/CollisionRules/CollisionRules.cpp` |
| React to a hit or a contact | `subscribe<MissileHit>` or `subscribe<ShipContact>` on the `EventBus`; the structs are in `src/game/Events/` |
| Change when two boxes count as overlapping | `overlaps` in `src/engine/Collision/Overlaps.hpp` |
| Change how entities move | `MovementSystem` in `src/game/Systems/MovementSystem/` |
| Add a component type | A new struct in `src/game/Components/` |
| Write a test with entities that collide | `tests/CollisionScene.hpp` |
