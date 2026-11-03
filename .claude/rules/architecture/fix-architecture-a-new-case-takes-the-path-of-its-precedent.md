---
description: "A new case takes the path its closest precedent takes; a second path for the same thing is put to the user in the plan, with the existing one"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: A new case takes the path of its closest precedent — a second path is a choice for the user

Why: a message for a game module got a new way in, the dispatcher handed to the modules, while `PlayerInput` already reached them through a hook; the plan never showed the existing way, and the user had the code changed after the pull request was open.

## Rule

1. **Before adding a way for one part to reach another, find the closest case the repository already handles** and follow its whole path, file by file: how `PlayerInput` goes from `GameSessions` to a module, how a service reaches a game. The written tutorial of that kind of change counts as a precedent.
2. **Follow that path by default.** Fewer lines is not a reason to leave it.
3. **A second path goes in the plan as a choice**, next to the existing one, each with what it adds and what it costs, for the user to decide. Never as the only option, and never only as "this also serves later cases".
4. **An argument of the form "X has no business knowing Y" is checked against what X already knows.** List what X already handles of the same kind before writing it.

### The precedent gives the shape of the code too

1. **Code registered next to others is written like them**: a handler of `GameSessions` is a `[this]` lambda that reads members, so a callback it calls is a member, never a `std::function` moved through a helper into the lambda.
2. **A helper that only exists because the shape changed is the symptom.** clang-tidy 21 reported `clang-analyzer-cplusplus.NewDeleteLeaks` on such a helper and stopped reporting it once the callback was a member; no leak was shown, and the finding fails the gate all the same.

### A control beside others acts like them

1. **Before writing a control for a row that already holds controls** (a chip beside HELP and BACK), read how each of them reacts: to the press or to the release, with a hovered and a pressed look or without.
2. **The new control takes their mechanism.** When they already differ, the difference goes to the user with the choice of one mechanism, never a third one beside them.

## Example

- ❌ **Before (wrong)**: `GameContext::messages` hands the `MessageDispatcher` to the modules, so `ResultsModule` registers `LeaderboardRequest` itself, "so `Sessions` need not know this message"; `GameSessions` already reads every message of a player.
- ✅ **After (right)**: `GameSessions` registers the message and calls `onLeaderboardRequest`, and `SessionHooks::leaderboardRequested` reaches the module, as `onNewerInput` and `newerInput` do for `PlayerInput`.
- ❌ **Before (wrong)**: `leaderboardRequestHandler(roster_, std::move(parts.onLeaderboardRequest))`, a helper returning a lambda that owns the callback.
- ✅ **After (right)**: `onLeaderboardRequest_`, a member, called by a `[this]` lambda like the handlers next to it.
- ❌ **Before (wrong)**: RANKING written as a new `KeyChip`, which acts on the release with a hovered and a pressed look, between HELP, a `Button` that acts on the press, and BACK, a chip of `Menu`: three mechanisms in one row, found by a review.
- ✅ **After (right)**: the two mechanisms of the row reported with the plan, and one chosen by the user for every chip of the row.
