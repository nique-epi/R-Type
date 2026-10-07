# RULE: An implementation plan shows all the code, explains each block simply, and leaves the on-screen test to the user

## Context

The rule was written during earlier work on another project, where plans were reviewed by the user before any code was written, and where the user tested the result themselves on the running application. It was imported into R-Type as a general deliverable-format rule; its original wording was specific to that project's language and tooling and was generalized here.

## Mistake

Plans described changes instead of showing them ("add a method to the tab model"), justified them with a list of technical bullets, and included a step in which the agent would drive an emulator to verify the behavior. The user could not validate the plan on code they had not seen, and the verification they would actually run was not given to them.

## Root cause

A plan was treated as a summary of intent rather than as the code review that precedes the code. The on-screen check was treated as an agent task although the user is the one who judges what appears on screen.

## Rule

1. All added or modified code appears in the plan, file by file. Only unchanged code may be summarized.
2. Every code section follows the template Why / What we do / What it does / code / Reading the code, symbol by symbol.
3. The on-screen check is a procedure for the user (action → expected on screen → expected in the logs); the agent keeps build, lint and unit tests. Exception: the user explicitly asks the agent to run and observe the application.
4. An API behavior the code relies on is cited with its source.

## Example

- ❌ **Before (wrong)**: "add `isReady()` to `Lobby`" with no code and a section "check by launching two clients".
- ✅ **After (right)**: the full code of `isReady()`, the template sections, and a table of checks the user runs in the game.

## Update (2026-10-06) — plan code formatted by the tool, predictions measured

The plan for scaling the playfield to the client window showed its C++ wrapped by hand. Two statements of `GameWindow.cpp` were wrapped differently from what `clang-format` produces, so the approved code failed `format-check` as written and had to be reformatted after approval. The same plan stated that inverting the ratio comparison would turn four tests red; run, it turned three red. The fourth only checks that the rectangle keeps the proportions of the playfield, which stays true when the rectangle overflows the window, and the overflow is what another test catches. Both were stated as facts in a document the user approves.
