---
description: After opening a PR, follow its CI state through the app's PR tools and report the real result
trigger: always_on
---

# RULE: Opening a PR is not the end — follow its CI until it is green or reported

Why: the user asked for it; a PR announced as done while its CI is red or unread hides the failures GCC and MSVC would show.

## Rule

1. **Right after `gh pr create`**, call `mcp__ccd_pr__get_status`. If it does not report that PR, bind it with `mcp__ccd_pr__bind_pr`.
2. **Read the checks**: counts, failing check names, mergeability, review decision. Never infer them from the local build (see `fix-reasoning-check-the-delivered-artifact-not-its-footprint.md`).
3. **Never poll CI myself**: no `ScheduleWakeup`, `CronCreate`, `/loop`, `Monitor` or `gh pr checks` loop. Re-read `get_status` when the user or a `<ci-monitor-event>` brings me back.
4. **Offer Auto-fix** (`mcp__ccd_pr__set_monitor`) in the answer, but only switch it on when the user agrees. Never enable auto-merge unless asked.
5. **On a failing check**, read its log (`gh run view <id> --log-failed`), fix on the PR's branch, push, and re-read the status. A check pending at the time of the answer is reported as pending, never as green.
6. **The final answer states the CI state** of every PR opened or pushed to in the turn: passing, failing (which checks) or pending.
7. **A PR I pushed to again** (new commits) is followed the same way.

### A pull request is built merged with its base

1. **The CI of a pull request builds `refs/pull/<n>/merge`**, the branch merged with the current base, never the branch alone. A name the branch renamed and that new code on the base still uses compiles on the branch and fails on the merge, while git reports the PR as mergeable: the conflict is in meaning, not in text.
2. **Before every push to a PR, compare its merge base with the current base** (`git fetch origin main`, then `git merge-base HEAD origin/main` against `git rev-parse origin/main`). When the base moved, merge it in a detached worktree (`git worktree add --detach <scratchpad>/<name> HEAD`, then `git merge --no-commit origin/main`), search the merged tree for every name the branch renamed or removed, build and test the merge, and push only once it is green. Syncing the PR's branch with the base follows the user's choice (merge or rebase).
3. **A conflict announced earlier ("main moved, a conflict is expected") is checked before the push**, never left for the CI to show.
4. **A conflict resolved toward the side that moved the code keeps every line the other side added to the old place.** When the branch moved code out of a function (a list of `provide` calls into a `provideParts()`) and the base added a line to that function, keeping the branch side drops the line, and the merge builds and passes the tests: nothing starts the whole program. Before resolving such a hunk, list what the base added there, above all a `provide`, `require`, `find` or registration (`git diff <merge base> origin/main -- src | grep -E '^\+.*context\.(provide|require|find)'`), and check that each one is in the merged tree (`git grep -n -F '<the call>' -- src`). A line without a test that would miss it gets one.

## Example

- ❌ **Before (wrong)**: "Draft PR opened, tests green locally." — the Windows job fails and nobody looks.
- ✅ **After (right)**: `get_status` → 1 failing check `build-and-test (windows)` → `gh run view <id> --log-failed` → MSVC `C4267` → fix, push, re-read; the answer says what is failing, pending or passing.
- ❌ **Before (wrong)**: three commits pushed to #103 after `main` gained #105, whose two new tests name the fixture `ServerConnectionTest` that the branch had renamed `ServerConnectionFixture` → Linux, Windows and clang-tidy fail on the merge, the PR still "mergeable".
- ✅ **After (right)**: `origin/main` merged in a detached worktree, the renamed names searched in the merged tree, the two suites renamed in the merge commit, the merge built and tested (1803/1803), then pushed.
- ❌ **Before (wrong)**: #227 merged `main` after #219, which added `context.provide<IServerTicks>(connection_.roundTrips())` to `ClientConnectionModule::install()`; the branch had moved that list into `PlayerConnection::provideParts()`, the hunk was resolved by keeping `provideParts()`, the merge built and 3991 tests passed, and the client would have stopped at its start on `require<IServerTicks>()`.
- ✅ **After (right)**: the grep of point 4 prints the `provide<IServerTicks>` line, `provideParts()` gains it, and `ProvidesTheClockOfTheServer` fails without it.
