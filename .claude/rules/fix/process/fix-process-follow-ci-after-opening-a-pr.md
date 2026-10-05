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

## Example

- ❌ **Before (wrong)**: "Draft PR opened, tests green locally." — the Windows job fails and nobody looks.
- ✅ **After (right)**: `get_status` → 1 failing check `build-and-test (windows)` → `gh run view <id> --log-failed` → MSVC `C4267` → fix, push, re-read; the answer says what is failing, pending or passing.
