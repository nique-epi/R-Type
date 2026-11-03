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
