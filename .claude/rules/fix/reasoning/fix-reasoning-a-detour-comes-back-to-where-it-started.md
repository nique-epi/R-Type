---
description: "A screen that sends the player through an existing flow gets them back: each entry of a shared flow is walked to its end, and takes its end from the entry"
trigger: always_on
---

# RULE: A detour comes back to where it started — each entry of a shared flow is walked to its end

Why: a flow written for one entry ends where that entry wanted to go, and a second entry inherits that end without anyone choosing it.

## Rule

1. **Before a screen sends the player into a flow that already exists** (the connection, the options, a dialog), write where that flow ends, and check that it is where this player is going. Walk it to its last screen, not only to its first.
2. **A detour keeps the screen it left under it**: `push()`, never `pop()` then `push()`, so the BACK of the flow comes back too.
3. **The flow takes its end from the entry** (a goal, a place, as `LobbyPlace` does), and each entry has a test of where it ends.
4. **What a detour opened for a screen is closed with that screen** when nothing else holds it: a session opened from the leaderboard on the title ends when the leaderboard closes.
5. **A request made before what it needs exists is kept**, and goes out once it does: the leaderboard asked before the session opened, and it must not be asked again by hand.

## Example

- ❌ **Before (wrong)**: JOIN A SERVER on the leaderboard does `pop()` then `push(CONNECTION_SCREEN)`; once accepted, the player lands in the waiting room, far from the scores they came for, and BACK of the form leads to the title.
- ✅ **After (right)**: JOIN A SERVER pushes `CONNECTION_FOR_LEADERBOARD_SCREEN` over the leaderboard; once accepted, that screen closes, and the leaderboard shows the scores, its request sent when the session opened; its BACK then leaves the server.
