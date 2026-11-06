# RULE: State kept per seat ends with the player who held the seat

## Context

The server answers a player's leaderboard request at most once per 500 ms, so a flood costs one read. `LeaderboardDesk` keeps the tick of the next answer allowed in a table of `MAXIMUM_PLAYERS` entries, indexed by player number.

## Mistake

Nothing cleared an entry when its player left. A player admitted on the seat of one answered just now inherited the wait and had its first requests ignored for up to 500 ms. The plan named the case ("a new player of the same number inherits at worst 500 ms of wait") and kept it; a review found it.

## Root cause

The seat number was taken as the identity of the player. It outlives the player: the next session on the seat gets the same number.

## Rule

1. Before indexing state by a seat or a player number, find where the seat is freed and clear the entry there.
2. Walk two players on one seat through it, and write what the second one gets.
3. The seat number and the address outlive the player; the session token is its identity.

## Example

- ❌ **Before (wrong)**: `nextAnswerTicks_` never cleared.
- ✅ **After (right)**: `onPlayerClosing()` called from the `closing` hook clears the seat, and `GivesTheNextPlayerOfASeatNoWait` asks with a second session on the seat in the same tick.

## Update (2026-11-06) — The test of a second player on a seat seats it from another address

### Context

The chat relay of a game (`src/server/Chat/ChatRelay`) keeps the newest line number and the wait of each seat, and clears them when the session closes. `ANewPlayerOnAFreedSeatStartsAfresh` checks that a newcomer on a freed seat gets its line relayed at once.

### Mistake

The test seated the newcomer at `FIRST_SEAT`, the same index, so the same address as the player who left, then asserted that `linesTo(FIRST_SEAT)` held one line. It held two: `linesTo` reads `sentTo(sent, playerEndpoint(index))`, every datagram ever sent to that address, the line of the first player included. The test failed on a correct relay.

### Root cause

The test identified a player by its index, which the fixture turns into an address, while the rule says an address outlives the player. The roster gives the lowest free number whatever the address, so another address takes the same seat.

### Rule added

The second player of such a test comes from another address, the test asserts it took the freed number, and a helper that reads by address is known to mix the sessions of one address.
