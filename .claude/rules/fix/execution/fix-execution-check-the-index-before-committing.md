---
description: Check the index before committing and the produced commit afterwards — a failing git add stages nothing
trigger: always_on
---

# RULE: Check the index BEFORE committing — a failing `git add` stages nothing at all

## Rule

1. **A `git add` with an invalid path fails for all paths**: `pathspec did not match` = nothing is staged. Never re-`add` a path already removed with `git rm`; to sweep wide, `git add -A -- <folders>`.
2. **Check the index before committing**, every time:
   ```bash
   git diff --cached --no-renames --name-status
   ```
   The list must match the intent exactly (compare paths, not a count). An `add` and a `commit` are not chained blindly.
3. **Check the produced commit** right after: `git show --stat --format= HEAD`.
4. **Never switch branches with uncommitted changes** that belong to the current branch: `git status --porcelain` first, otherwise the changes silently travel to the other branch.
5. **Never commit build artifacts**: `build/`, `r-type_server` / `r-type_client` binaries at the root, `compile_commands.json`, `vcpkg_installed/`. If one shows up in `git status`, the `.gitignore` is what needs fixing.
6. **`git checkout -- <file>` destroys** the file's uncommitted changes, with no safety net.

### A path staged earlier rides along with the next commit

1. **`git add <paths>` adds to the index, it never empties it.** A `git rm`, `git mv` or `git add` run earlier for another commit is still staged, and the next `git commit` takes it with the rest.
2. **Read the index in one call, commit in the next.** `git add` and `git diff --cached --no-renames --name-status` go together; the `git commit` waits for the next call, once every listed path has been matched with the commit's intent. A chain that prints the index and commits in the same call only shows the stray path after it is committed.
3. **When one working tree prepares several commits**, a `git rm` for a later commit is unstaged (`git restore --staged <path>`) before an earlier commit is made, or run only once that earlier commit exists.

## Example

- ❌ **Before (wrong)**: `git rm old.cpp && git add old.cpp new.cpp CMakeLists.txt && git commit -m "…"` → `pathspec 'old.cpp'` → commit containing only the deletion.
- ✅ **After (right)**: `git rm old.cpp` → `git add -- new.cpp CMakeLists.txt` → `git diff --cached --no-renames --name-status` (D, A, M expected) → `git commit` → `git show --stat`.
- ❌ **Before (wrong)**: `git rm -r StandInServerConnection/` for the client commit, then `git add <refactor files> && git diff --cached --name-status && git commit` → the refactor commit also deletes the stand-in.
- ✅ **After (right)**: `git add <refactor files> && git diff --cached --no-renames --name-status` → four `D` lines the refactor does not announce → `git restore --staged` them → commit in a second call.
