---
description: Each step of a sequence requests an absolute state and checks its result; it never assumes where the previous step left it
trigger: always_on
---

# RULE: A multi-step sequence ASSERTS its starting state — it never assumes where the previous step left it

## Rule

1. **Every step names the state it requires, in absolute terms**: `setState(GameState::Playing)`, never `nextState()`. Idempotent: already in the requested state ⇒ immediate success.
2. **The first step re-asserts**, even when initialization "already did the job": initialization runs once, the sequence as many times as it is replayed (new game, reconnection, next level).
3. **A failing transition interrupts the sequence**: it returns a result the caller checks, not a `void` with an error log while the sequence carries on.
4. **Replaying is the nominal case.** Facing any state kept between two runs (game in progress, packet queue, sequence counter), ask: *what does the second run look like if the first stopped halfway?*
5. **A silent inversion is worse than a failure**: when two values of the same type differ only by their origin (source/destination, old/new, client/server), guarantee the origin through the type or the construction.

## Example

- ❌ **Before (wrong)**: `toggleReady()` on each click on "Ready" → one lost packet inverts the state between client and server for the rest of the game.
- ✅ **After (right)**: the client sends `setReady(true)` / `setReady(false)`, an absolute state, and the server sends back the applied state.
