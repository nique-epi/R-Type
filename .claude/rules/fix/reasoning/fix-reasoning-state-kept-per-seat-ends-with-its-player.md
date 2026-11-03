---
description: "A table indexed by seat or player number is cleared when the seat is freed, so the next player on it never inherits the state of the one who left"
trigger: always_on
---

# RULE: State kept per seat ends with the player who held the seat

## Rule

1. **Before indexing state by a seat or a player number** (a wait, a counter, a last value per player), find where the seat is freed (`SessionHooks::closing` on the server) and clear the entry there.
2. **Walk two players on one seat through it**: the first sets the entry and leaves, the second arrives on the same number. Write what the second one gets; anything but the state of a newcomer is the defect.
3. **A key that outlives the player (the seat number, the address) is never the identity of the player**: the session token is. When the state belongs to the player, clear it on departure or key it by the token.

## Example

- ❌ **Before (wrong)**: `LeaderboardDesk::nextAnswerTicks_`, one wait per seat, never cleared: a player who took the seat of one answered just now waited up to 500 ms for nothing, and the plan named it and kept it.
- ✅ **After (right)**: `LeaderboardDesk::onPlayerClosing()` clears the wait of the seat from the `closing` hook, and a test seats a second session on that seat and sees its request go through in the same tick.
