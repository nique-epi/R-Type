---
description: Merge conflicts — resolve mechanical conflicts alone, stop and notify as soon as a conflict is big or ambiguous
trigger: always_on
---

# RULE: Resolve mechanical merge conflicts alone, NOTIFY as soon as a conflict is big or ambiguous

## Rule

On every `git merge` / `git rebase` / `git pull` that produces conflicts, **classify each conflict before touching it**, then apply the rule of its category.

### "Mechanical" conflicts — resolve alone

The right resolution can be deduced without arbitration:

- one side is a strict **superset** of the other;
- **rename vs edit** of the same symbol, where both intents compose;
- **accumulation** files (list of sources in a `CMakeLists.txt`, dependency list, README): the resolution is the **union** of both sides;
- purely textual conflict on a comment or doc comment.

After resolution: build and tests green before committing the merge.

### "Big" conflicts — STOP, notify, wait

Stop as soon as a single signal is present:

- both sides changed the **same logic** in different directions;
- the conflict is about a **contract**: public signature, interface, packet format, packet type id, CMake preset, vcpkg baseline;
- resolving would require **deleting code from the other side** whose intent is unknown;
- more than ~10 hunks, or a hunk of more than ~40 lines;
- the slightest doubt about what the other side was trying to do.

In that case: do not resolve, leave the merge in progress (or `git merge --abort` if the state is confusing), and lay out for each disputed conflict the file, what each side does, and the recommended option.

### Always

- **Never `git checkout --ours/--theirs` on a whole file**: it also throws away the auto-merged hunks of that file. Edit the markers.
- **Never commit a file still containing `<<<<<<<`**: `git diff --check` and `grep -rn '^<<<<<<<'` before committing.
- **Report the outcome**: how many conflicts, in which files, resolved how.

### A union reads the lines around the block

1. **Before joining both sides of a block, read the line just above and just below it.** When both sides start inside a shared opening (`/**`, `{`, a list) or end before a shared closing (`}`, `*/`), each side holds half of an item: the union must put the closing and the opening back between them (`}`, a blank line, `/**`).
2. **A union is checked by compiling the file**, never by the absence of markers: a missing `}` leaves no marker and turns every following test into a local class.

## Example

- ❌ **Forbidden**: `git checkout --ours src/server/CMakeLists.txt` to get rid of the markers → the sources added by the other branch disappear from the build.
- ✅ **Instead**: edit the hunk keeping the union of both source lists, build, commit, and say what was resolved.
