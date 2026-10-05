---
description: An implementation plan shows all the new code, explains each block simply, and leaves the on-screen check to the user
trigger: always_on
---

# RULE: An implementation plan shows all the code, explains each block simply, and leaves the on-screen test to the user

Why: the user validates the plan on the code they will read, not on a description of it — and they are the one who tests it in the running game.

## Rule

1. **All added or modified code appears in the plan**, file by file: signatures, bodies, doc comments, includes, tests. Only unchanged code may be summarized (`// … unchanged`), never new code.
2. **Every code section follows this template, in this order:**
   - **Why.** — one to three plain sentences, no jargon: the need, not the mechanics.
   - **What we do.** — as short as the why: the change itself.
   - **What it does.** — the resulting behavior.
   - the code block;
   - **Reading the code, symbol by symbol.** — for each function, what it returns (a truth table for a boolean); for each identifier not declared in the block, where it comes from (file:line, who provides it, who reads it).
3. **The on-screen check is a procedure for the user.** The plan does not include driving the game client or a window to observe it: it delivers a table *action → expected on screen → expected in the logs*, and the PR waits for the user's validation. The agent keeps the build, `format-check`, `tidy` and the unit tests. Exception: the user explicitly asks the agent to run and observe the game.
4. **An API behavior the code relies on is cited with its source** (headers installed by vcpkg, official documentation of the pinned version), in the plan as in the code.

## Example

- ❌ **Before (wrong)**: "add `isReady()` to `Lobby`" with no code, a "Why" made of six technical bullets, and a section "check by launching two clients".
- ✅ **After (right)**: the full code of `isReady()`; "Why: the server needs a rule that says when a game may start"; "What we do: add `isReady()` to `Lobby`"; a reading that says what it returns and where `players_` and `minimumPlayers` come from; a table of checks the user runs in the game (action → expected on screen → expected in the server log).
