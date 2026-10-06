---
description: Check the delivered artifact itself (binary, CI run, PR state), never an indirect footprint; do not invent an explanation to save a claim
trigger: always_on
---

# RULE: Check the delivered artifact itself, never an indirect footprint

## Rule

When a conclusion is about **what was delivered** (a binary, a CI run, a PR, a remote branch):

1. **Open the artifact itself.** A binary is checked by running it (and `ls -l` on the file produced at the repository root), a CI run by its log (`gh run view <id> --log-failed`), a PR by its state (`gh pr view <n> --json state,headRefOid,mergeable`). Never inferred from an intermediate folder or an exit code.
2. **Never draw proof of absence from a single location** without listing where the thing can legitimately be (repository root, `build/`, `Release/` with the Visual Studio generator).
3. **When the observation contradicts the user, do not invent the bridge**: say it ("what I see contradicts X"), then look for the measurement that settles it.
4. **After any operation on a branch that carries an open PR** (force-push, rebase, rename), check the PR's state. Never rename or delete a branch with an open PR.
5. **Before attributing a write to a tool** (clang-format, CMake, vcpkg), prove who wrote it: `git log -- <file>`.
6. **A tool's index is not the disk**: before claiming a space is purged, list the medium itself (`git worktree list` **and** the folder, `git branch` **and** `git ls-remote`).

### A pull request's state is read before it is stated

1. **Before writing that a PR is open, kept, merged or closed**, read it in the same turn: `gh pr view <n> --json state,closedAt,mergedAt`. A state remembered from an earlier turn is not a state: the user may have closed or merged it meanwhile.
2. **When the user answers a question about a PR** ("keep or close?"), read its state before acting on the answer: the question may already be settled on GitHub.

## Example

- ❌ **Before (wrong)**: "the Windows CI passes" because the Linux job is green and the code has nothing platform-specific.
- ✅ **After (right)**: `gh pr checks <n>` → the Windows job fails → `gh run view <id> --log-failed` → MSVC warning `C4267` treated as an error → targeted fix.
