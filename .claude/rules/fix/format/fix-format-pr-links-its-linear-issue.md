---
description: A pull request links its Linear issue with a closing magic word and the issue ID in its description, then the link is checked on the Linear side
trigger: always_on
---

# RULE: A pull request links its Linear issue — `Closes RTY-<n>` in the description, then the link is checked

Why: Linear's GitHub integration only links a PR that names the issue the way Linear documents it; "Covers MO-05 (RTY-64)" in PR #39 linked nothing.

## Rule

1. The `## Summary` of a PR that implements a Linear issue holds the line `Closes RTY-<n>`: a closing magic word (`Closes`, `Fixes`, `Resolves`, `Completes`, `Implements`) followed by the issue ID. A mention without a magic word ("Covers", "for", an ID in parentheses) is not a link.
2. A PR that delivers only part of the issue uses a non-closing magic word instead (`Part of RTY-<n>`, `Refs RTY-<n>`), so its merge does not move the issue to Done.
3. The magic word goes in the PR description or title, never in a PR comment (comments create no link) and never in a commit message (`commit.md` forbids ticket references).
4. Right after opening the PR, read the issue on the Linear side (`get_issue` with `attachments`): the PR URL must be listed. Report the status as read, never as expected.
5. Statuses then follow the team automation (Linear: Settings > Team > Workflows & automations). On this workspace, PR #44 marked ready for review moved RTY-81 to In Progress, its review request to In Review, its merge to Done, each within 2 seconds.

## Example

- ❌ **Before (wrong)**: "Covers MO-05 (RTY-64)." in PR #39 → RTY-64 lists no PR.
- ✅ **After (right)**: "Closes RTY-81 (Linear)." in PR #44 → RTY-81 lists PR #44 and followed its review and merge.
