# RULE: The rules in force are the ones on disk — reread them after switching branch

## Context

The session started on `docs/project-layout`, a branch already merged and 19 commits behind `origin/main`; its rules were loaded into the session then. To implement the client screen stack, the work moved to `feat/screen-stack`, created from `origin/main`, whose `.claude/` had gained and extended about twenty rules in the meantime, among them `fix-execution-zero-warnings-proven-on-a-recompiling-build.md` with a section on red proofs.

## Mistake

The new asset tests were proven red by four mutations of `AssetLibrary.cpp` and `AssetLoadingStep.cpp`, each restored with `git checkout -- <file>`. The object file of the last mutation was never deleted. The restore happened in the same second as that build, `make` (GNU Make 3.81) saw the object as up to date, and the next test run used the mutated library: four new loading screen tests failed while the code was right. Deleting the object file and rebuilding made the 17 screen tests and the 76 client tests pass.

## Root cause

The rule that prescribes deleting the object file before each variant's build, including the restored original, was on disk but not in the session's context: the session held the copy loaded from the older branch, and the rules were not reread after switching to a branch started from a newer `main`.

## Rule

1. Right after creating or checking out a branch, compare `.claude/` with the commit the session started on: `git diff --stat <session start commit> HEAD -- .claude/`.
2. Read every rule the diff adds or changes, and the index in `.claude/CLAUDE.md`, before writing the first line of code.
3. Apply the newest version: when a prescription in the session's context and the file on disk differ, the file wins.

## Example

- ❌ **Before (wrong)**: session started on a merged branch, then `git checkout -b feat/screen-stack origin/main` → a red proof restores a mutated source without deleting its object file → the next tests run the mutated binary.
- ✅ **After (right)**: after the checkout, `git diff --stat c84d96e HEAD -- .claude/` lists the changed rules → read them → the red proof deletes the object file before each build, including the build of the restored original.
