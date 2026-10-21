---
description: "A function does one kind of step: the checks that decide whether to go on and the writes that build go in separate functions"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: A function does one kind of step — the checks that decide and the writes that build go in separate functions

## Context

Pull request that makes the client game screen show the entities the server announces. `EntityReplicator::create()` had grown with the change: besides creating the entity from its type, it now gave it its `EntityType`, its animation and a sprite on the frame that animation starts on, then played the appearance effect.

## Mistake

A teammate reviewing the pull request commented "fonction longue" on `EntityReplicator.cpp`, anchored on the first line of `apply(EntitySpawned)`, just above `create()`.

`create()` had 21 lines of body and did three kinds of step:
- three checks with early returns: the reserved id `NO_ENTITY_ID`, a type outside the protocol, a type without a sprite;
- six writes: create the entity, add its `Position`, `EntityType`, `Sprite` and `SpriteAnimation`, bind its network id;
- one notification: play its appearance effect.

`apply(EntitySpawned)` also copied the four fields of the message into an `EntityState` inline.

The three-parameter rule had been measured on every changed function, and the code was clean for format, clang-tidy and tests: nothing in the gates looks at what a body does.

## Root cause

No rule asked me to look at the steps of a body. I grew `create()` by adding writes to a function that already checked, because each addition was one line, and I measured parameters and warnings, never the shape of the body.

## Rule

1. **Before shipping a function written or changed, name the kinds of step its body does**:
   - checks that decide whether to go on: early returns on a reserved id, an unknown type, a failed lookup;
   - construction that writes: create an entity, add its components, bind an id;
   - notification: play an effect, send a message, log.
   A body that does more than one kind is split. Sources: C++ Core Guidelines F.2, "A function should perform a single logical operation", and F.3, "Keep functions short and simple".
2. **Split along the steps**:
   - the checks become a function that returns the validated value as a `std::optional`, nothing meaning "do not go on": `creatableTypeOf(announced)`;
   - the writes become a function that takes the validated values and returns what it built: `addEntity(type, position, look)`;
   - a conversion written inline (a designated initializer that copies one message into another type) becomes a named function: `announcedBy(message)`;
   - the caller then reads as its sequence: check, build, bind, notify.
3. **The split keeps the guarantees of the original**: every check still runs before the first write, and each new function respects `fix-architecture-at-most-three-parameters.md`.
4. **A body longer than 15 lines is the signal to apply point 1**, not a limit by itself: reread it before the push and name its steps.
5. **A body that repeats one kind of step is one step**: a list of handler registrations, or a `switch` that maps each value of an enum, is not split for its length alone.

## Example

- ❌ **Before (wrong)**: `create()` checks the id, maps the type, looks up the sprite (three early returns), then creates the entity, adds four components, binds the id and plays the appearance: 21 lines of body, three kinds of step.
- ✅ **After (right)**: `create()` asks `creatableTypeOf()` and `animatedSpriteOf()`, then binds the id to what `addEntity()` built and plays the appearance: 13 lines of body; `addEntity()` holds the writes, `creatableTypeOf()` the checks, and `apply(EntitySpawned)` calls `announcedBy()` instead of copying the message field by field.
