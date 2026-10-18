---
description: An implementation plan shows all the new code, a usage example of each new feature, explains each block simply, and leaves the on-screen check to the user
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
   - **How it is used.** — an example of a caller, in code, with the real names: what a reader writes to use the new class, function, constant or message. Where the plan itself holds the caller, say where (the section and the line) instead of repeating it. A caller that is not part of the change is marked as an illustration, and the plan says which code does call the feature today.
   - the code block;
   - **Reading the code, symbol by symbol.** — for each function, what it returns (a truth table for a boolean); for each identifier not declared in the block, where it comes from (file:line, who provides it, who reads it).
3. **The on-screen check is a procedure for the user.** The plan does not include driving the game client or a window to observe it: it delivers a table *action → expected on screen → expected in the logs*, and the PR waits for the user's validation. The agent keeps the build, `format-check`, `tidy` and the unit tests. Exception: the user explicitly asks the agent to run and observe the game.
4. **An API behavior the code relies on is cited with its source** (headers installed by vcpkg, official documentation of the pinned version), in the plan as in the code.
5. **Every new feature has its usage example, with no exception for "internal" ones**: a constant, a message struct, an exception, a helper. The example shows the call in its context (who calls it, with what), is formatted by `clang-format` like the rest of the plan's code, and uses names that exist in the plan or the repository. A feature that nothing calls yet says so: "no caller in this change; the tests call it".

### The code of a plan went through the tools, and a prediction is called a prediction

1. **Code shown in a plan is formatted by `clang-format`, not wrapped by hand**: write it to a scratch file, run `clang-format --assume-filename=<its future path>` on it, and paste the result. A line wrapped by counting columns is not a formatted line.
   Feed the scratch file on standard input, from the repository root: `clang-format --style=file --assume-filename=<its future path> < <scratch file>`. Given as an argument, a file outside the repository is formatted with the style found from its own folder, which is clang-format's default, not the project's.
2. **Say which `clang-format` produced it** (`clang-format --version`), and whether it is the version the CI runs.
3. **"These tests will fail without the fix" is a prediction until it is run.** Write it as one in the plan, then report the measured list, including the tests that did not behave as predicted and why.

## Example

- ❌ **Before (wrong)**: "add `isReady()` to `Lobby`" with no code, a "Why" made of six technical bullets, and a section "check by launching two clients".
- ✅ **After (right)**: the full code of `isReady()`; "Why: the server needs a rule that says when a game may start"; "What we do: add `isReady()` to `Lobby`"; a reading that says what it returns and where `players_` and `minimumPlayers` come from; a table of checks the user runs in the game (action → expected on screen → expected in the server log).
- ❌ **Before (wrong)**: a section with the code of `WorldStateFilter` and its truth table, and nothing showing the client calling it before it applies a state.
- ✅ **After (right)**: the same section with "How it is used": the four lines of the client that decode, ask `accept(state.tick)`, then apply, marked as an illustration because the client screen that receives states is another issue.
