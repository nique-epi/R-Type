---
description: After any command that moves HEAD, read every rule changed since the session started before writing code; the newer version of a rule wins
trigger: always_on
---

# RULE: The newest rules are in force — reread the changed ones after HEAD moves

Why: the rules loaded at session start come from the branch checked out then; a branch started from a newer `main` can carry rules the session never saw.

## Rule

1. **Right after any command that moves HEAD to other commits** (`checkout`, `switch`, `pull`, `merge`, `rebase`), compare `.claude/` with the commit the session started on: `git diff --stat <session start commit> HEAD -- .claude/`. The session start commit is the one shown in the session's first git status.
2. **Read every rule the diff adds or changes**, and the index in `.claude/CLAUDE.md`, before writing the first line of code. A rule on disk binds even if the session's context holds an older copy.
3. **The newer version of a rule wins**: when the session's copy and the file on disk differ, apply the one from the more recent commit. A branch older than the session start does not bring back an older rule: the rules of `origin/main` still apply.

## Example

- ❌ **Before (wrong)**: session started on a merged branch, then `git checkout -b feat/screen-stack origin/main` (19 commits ahead, rules extended) → a red proof restores a mutated source without deleting its object file, a step the newer rule prescribes → the next tests run the mutated binary and four tests fail for no reason in the code.
- ✅ **After (right)**: after the checkout, `git diff --stat c84d96e HEAD -- .claude/` lists the changed rules → read them → the red proof deletes the object file before each build, including the build of the restored original.
