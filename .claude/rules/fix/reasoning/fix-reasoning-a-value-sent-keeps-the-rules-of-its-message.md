---
description: A new value written into a message is checked against the payload rules of that message, by a test that encodes it
trigger: always_on
---

# RULE: A value a change writes into a message keeps the payload rules of that message

Why: a level that opens in its biome told it began at -4096 units; `LevelStatus` refuses a negative `biomeSince`, so the server never sent the status and the client stayed under the sky of space.

## Rule

1. **Before giving a field a value no code gave it before** (a negative position, a zero, an empty text, a number past a bound), find the message that carries it to the other program and read its checks: the payload rules of `protocol.mdx` and the `validate()` of its codec in `src/network/Protocol/Messages/`.
2. **When the value breaks a rule, change one of them on purpose**: the value, or the rule with its codec, its tests and its page, in the same change. Never leave both.
3. **The test of the new value goes through the encoder**: build the event the way the code builds it and encode it (`eventMessageOf()`, `encodeMessage()`), so a refused value fails the test. A test that stops at the event of the game passes whatever the wire accepts.

## Example

- ❌ **Before (wrong)**: `LevelDirector` writes `biomeSince = -4096`; `ALevelOpensInsideItsOpeningBiome` checks the `LevelChange` alone; in the game, `requireScrollValue` refuses the status and nothing is sent.
- ✅ **After (right)**: `LevelStatus` accepts a finite `biomeSince`, its page says so, and `EncodesTheBiomeALevelOpensIn` encodes the change and decodes -4096.
