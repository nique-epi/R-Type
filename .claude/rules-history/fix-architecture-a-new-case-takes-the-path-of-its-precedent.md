# RULE: A new case takes the path of its closest precedent — a second path is a choice for the user

## Context

The per-server leaderboard adds `LeaderboardRequest`, a message of a player that a game module answers.

## Mistake

The plan gave `GameSessions` an accessor to its `MessageDispatcher` and `GameContext` a field `messages`, so `ResultsModule` could register the handler. `PlayerInput` already reached the modules through `GameSessionsParts::onNewerInput` and `SessionHooks::newerInput`, and `tutorial-add-a-network-message.md` says to register a handler in `GameSessions`. The pull request was open when the user asked why a dispatcher was needed at all. The explanation given was that `Sessions` need not know a message that does not concern it; the user pointed out that it knows every other message of a player.

## Root cause

The design started from the shortest way to reach the module, not from how a comparable message already gets there. The plan then showed that way alone, so the user could not compare.

## Rule

1. **Before adding a way for one part to reach another, find the closest case the repository already handles** and follow its whole path.
2. **Follow that path by default.**
3. **A second path goes in the plan as a choice.**
4. **An argument of the form "X has no business knowing Y" is checked against what X already knows.**

## Example

- ❌ **Before (wrong)**: `GameContext::messages` hands the dispatcher to the modules.
- ✅ **After (right)**: `onLeaderboardRequest` then `SessionHooks::leaderboardRequested`, as `onNewerInput` then `newerInput`.

## Update (2 November 2026) — The precedent gives the shape of the code too

The first version of the hook registered the handler through a free helper, `leaderboardRequestHandler()`, which moved a `std::function` into the lambda it returned, while the handlers next to it are `[this]` lambdas reading members. `clang-tidy` 21.1.8 reported `clang-analyzer-cplusplus.NewDeleteLeaks` ("Potential leak of memory pointed to by field '__f_'") at `MessageDispatcher.hpp:71` for `GameSessions.cpp`. No leak was shown: the report was not investigated further once a member callback, in the shape of the neighbours, made it disappear from the same command.

## Update (3 November 2026) — A control beside others

The waiting room shows three chips in one row. HELP was a `Button` the walkthrough opens on the press, without a hovered or a pressed look; BACK is a chip of `Menu`, which acts on the release. The leaderboard added RANKING as a new `KeyChip`, which acts on the release with both looks, so the row held three mechanisms. A review found it. The new subsection asks to read how the controls of a row react before writing one more, and to put an existing difference to the user instead of adding a third mechanism.
