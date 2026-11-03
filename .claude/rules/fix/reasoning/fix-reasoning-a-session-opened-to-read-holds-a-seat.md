---
description: "Before a client screen opens a session for anything but playing, walk through what an admitted session holds on the server: a seat, the waiting room of the others, the ready check, the refusals"
trigger: always_on
---

# RULE: A session holds a seat — a screen that opens one to read walks through what that seat does

## Rule

1. **Before a screen of the client opens a session for anything but playing** (reading a leaderboard, a list, a status), list what the server does with every admitted session: it takes one of `MAXIMUM_PLAYERS` seats and the nickname, appears in the `LobbyState` of the others, counts in `everyPlayerIsReady()`, and is refused while the game is full or in progress.
2. **Write what the other players see while that screen stays open**, then what the player of that screen gets from a full server and during a game. A session that keeps the others from starting, or a read refused while the server is busy, is the defect.
3. **A read that needs no seat takes a path that reserves none**, and its answer keeps the anti-amplification rule of the protocol (`fix-reasoning-an-exception-to-a-check-is-read-as-the-forger.md`). The choice between the paths goes to the user before the code.

## Example

- ❌ **Before (wrong)**: JOIN A SERVER on the leaderboard opened from the title opened a full session to read the leaderboard; the pilot took a seat, showed as a pilot not ready in the waiting room of three ready pilots, and their game could not start until it pressed BACK.
- ✅ **After (right)**: the plan walks the session through the server, the room of the others and a full server included, and puts to the user the ways to read a leaderboard with what each holds on the server.
