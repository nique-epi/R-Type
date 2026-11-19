# RULE: A value a change writes into a message keeps the payload rules of that message

## Context

A level may open inside its biome (`level.register(…, { biome = "fosse" })`). `LevelDirector` tells the biome began `OPENING_BIOME_LEAD` (4096 logical units) before the scroll, so a client draws the biome without its entrance.

## Mistake

The test checked the `LevelChange` the director publishes. The status goes to the clients as a `LevelStatus`, whose codec refuses a `biomeSince` that is negative (`requireScrollValue`). The server could not encode the status of such a level, sent none, and the user saw the sky of space with the mobs of the Pit.

## Root cause

The value was checked against what the game needed, never against the message that carries it. The rule of the wire was on the protocol page and in the codec, two steps away from the change.

## Rule

See `.claude/rules/fix/reasoning/fix-reasoning-a-value-sent-keeps-the-rules-of-its-message.md`.

## Example

- ❌ **Before (wrong)**: `biomeSince = -4096` checked on the `LevelChange` only.
- ✅ **After (right)**: the codec accepts a finite `biomeSince`, and a test encodes the change and decodes the value.
