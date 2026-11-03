# RULE: A session holds a seat — a screen that opens one to read walks through what that seat does

## Context

The leaderboard of a server is read with `LeaderboardRequest`, which the server accepts only from a player with a session. The leaderboard screen can be opened from the title; without a session, its JOIN A SERVER bar opens the connection screen, and the connection it makes comes back to the leaderboard.

## Mistake

That connection is a full session. A review walked it through the server: the pilot takes one of the four seats and its nickname, the `LobbyState` of the other players lists it as not ready, and `everyPlayerIsReady()` stays false, since its client sends heartbeats and it never becomes away. Three ready pilots cannot start until it presses BACK. The same pilot cannot read the leaderboard of a full server (`GameFull`) or of a server in a game (`GameInProgress`).

## Root cause

The session was taken as a way to talk to the server, not as a seat in a game. What the server does with an admitted session was never listed for a screen that only reads.

## Rule

1. Before a screen opens a session for anything but playing, list what the server does with every admitted session.
2. Write what the other players see while the screen stays open, and what that player gets from a full or busy server.
3. A read that needs no seat takes a path that reserves none and keeps the anti-amplification rule; the choice goes to the user before the code.

## Example

- ❌ **Before (wrong)**: JOIN A SERVER from the leaderboard opened a full session, which blocked the start of the others.
- ✅ **After (right)**: the walk-through in the plan, and the ways to read a leaderboard put to the user.
