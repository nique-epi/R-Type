---
description: "A table indexed by seat or player number is cleared when the seat is freed, so the next player on it never inherits the state of the one who left"
trigger: always_on
---

# RULE: State kept per seat ends with the player who held the seat

## Rule

1. **Before indexing state by a seat or a player number** (a wait, a counter, a last value per player), find where the seat is freed (`SessionHooks::closing` on the server) and clear the entry there.
2. **Walk two players on one seat through it**: the first sets the entry and leaves, the second arrives on the same number. Write what the second one gets; anything but the state of a newcomer is the defect.
3. **A key that outlives the player (the seat number, the address) is never the identity of the player**: the session token is. When the state belongs to the player, clear it on departure or key it by the token.

### The test of a second player on a seat seats it from another address

1. **The second player of the test comes from another address than the first**, and the test asserts it took the freed number (`ASSERT_EQ(seat(SECOND_SEAT).playerNumber, freedSeat)`). The roster gives the lowest free number whatever the address, so the seat is the same.
2. **A helper that reads what was sent to an address (`sentTo(sent, playerEndpoint(index))`) counts the datagrams of every session that used it**: a second player on the same address reads the datagrams of the first too, and the test fails on correct code or passes on wrong code.

## Example

- ❌ **Before (wrong)**: `LeaderboardDesk::nextAnswerTicks_`, one wait per seat, never cleared: a player who took the seat of one answered just now waited up to 500 ms for nothing, and the plan named it and kept it.
- ✅ **After (right)**: `LeaderboardDesk::onPlayerClosing()` clears the wait of the seat from the `closing` hook, and a test seats a second session on that seat and sees its request go through in the same tick.
- ❌ **Before (wrong)**: `ANewPlayerOnAFreedSeatStartsAfresh` seated the second chat player at `FIRST_SEAT` again, the address of the first, and asserted one line in `linesTo(FIRST_SEAT)` → 2 lines, the line of the first player included, on a correct `ChatRelay`.
- ✅ **After (right)**: the second player is seated at `SECOND_SEAT`, its number asserted equal to the freed one, and `linesTo(SECOND_SEAT)` holds its line only.
