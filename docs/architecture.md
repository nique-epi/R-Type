# Architecture decision: engine, game and network libraries

## Context

The client renders with SFML, the server talks over the network with Asio, and both must run the same game logic. If rendering, networking and logic live in one place, no part can be changed or tested alone, and the server would be forced to link a graphics library.

## Decision

The code is split into three static libraries with one-way dependencies, and the game objects are organised as an **ECS** (Entity Component System).

```
r-type_client -> rtype_client_window -> SFML
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

## Consequences

- Only a minimal `EntityRegistry` exists today; components and systems are added by the next stories.
- Any new target declares its links explicitly; a link that breaks the table above fails the configure step.

## Validation

| Team | Status |
|---|---|
| Team 1 | pending |
| Team 2 | pending |
| Team 3 | pending |
| Team 4 | pending |
