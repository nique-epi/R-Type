# RULE: A pull request links its Linear issue — `Closes RTY-<n>` in the description, then the link is checked

## Context

The team tracks its stories in Linear (workspace `nique-epi`, issues `RTY-<n>`), and Linear's GitHub integration is installed on the repository. The plan for the engine event bus (RTY-65) described the future pull request without saying how it would be linked to its issue.

## Mistake

The user had to point out that the PR description must link RTY-65 so that the issue is connected to the PR and its status follows it. The previous PR written the same way, #39 for RTY-64, said "Covers MO-05 (RTY-64)": RTY-64 lists no pull request.

## Root cause

`fix-format-pull-requests-in-english.md` asked for `Closes #` in the Summary, the form of a GitHub issue number. The team's issues live in Linear, which needs a magic word followed by the Linear ID, and nothing checked the link after opening.

## Verification

- Linear documentation (<https://linear.app/docs/github>): a PR is linked by the issue ID in its branch name, in its title, or by a magic word followed by the ID (or the issue URL) in its description or title. Closing words (close, fix, resolve, complete, implement and their forms) also apply the "On PR or commit merge" status at the merge; non-closing words (ref, part of, contributes to, toward) link without it. Magic words in PR comments create no link. By default, linked issues move to In Progress when the PR opens and to Done when it merges; the triggers (drafted, opened, review requested, ready for merge, merged) are configured in Settings > Team > Workflows & automations.
- This workspace: PR #44 ("Closes RTY-81 (Linear).") is attached to RTY-81, which moved to In Progress 2 s after the PR was marked ready for review, to In Review 2 s after the review request, and to Done 2 s after the merge. PR #48 ("Closes RTY-40.") is attached to RTY-40, which moved to In Review 1 s after the review request. PR #39 has no magic word and RTY-64 lists no PR.
- Not determined: the status a draft PR applies, which depends on the team's settings. RTY-64 moved to Done 2 s after #39 merged although no PR is attached to it; the cause is not known.

## Rule

1. The `## Summary` of a PR that implements a Linear issue holds the line `Closes RTY-<n>`.
2. A PR that delivers only part of the issue uses a non-closing magic word (`Part of RTY-<n>`).
3. The magic word goes in the PR description or title, never in a comment or a commit message.
4. Right after opening the PR, read the issue on the Linear side: the PR URL must be listed.
5. Statuses then follow the team automation.

## Example

- ❌ **Before (wrong)**: "Covers MO-05 (RTY-64)." → RTY-64 lists no PR.
- ✅ **After (right)**: "Closes RTY-81 (Linear)." → RTY-81 lists PR #44 and followed its review and merge.
