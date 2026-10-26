# RULE: A guard runs BEFORE the write it protects — a check placed after it is only an alarm

This rule was imported from another project when R-Type started; its original incident is not recorded here. This history starts with its first recurrence in this repository.

## Update (26 October 2026) — a check typed before the write, in the same call, without gating it

### Context

Opening the draft pull request of a batch of rules. No AI mention may appear in a pull request description (`fix-process-no-co-author-trailer.md`), so the description file was searched for one before `gh pr create`.

### Mistake

The search and the creation were sent in one call: `grep -n -i -E 'claude|anthropic|generated with|co-authored' pr-body.md; gh pr create --draft … --body-file pr-body.md`. The `;` let `gh pr create` run whatever `grep` printed, and the five matches were read only after the pull request existed. All of them were `.claude/` paths, so nothing forbidden was published; had one been a real mention, it would have been public before being seen.

### Root cause

The search ran before the write in time, which point 1 of the rule asks, but nothing made the write depend on it: the check was an alarm read after the fact. Its pattern also matched allowed words (`.claude/`), so it could not have gated the write as it stood; it needed reading first.

### Rule

1. A check in the same call as its write is the condition of that write (`if <check>; then <write>; …`).
2. A check whose output needs reading runs alone, and the write goes in the next call.

### Example

- ❌ **Before (wrong)**: `grep … pr-body.md; gh pr create … --body-file pr-body.md`.
- ✅ **After (right)**: `grep … pr-body.md` alone, its matches read, then `gh pr create` in the next call.
