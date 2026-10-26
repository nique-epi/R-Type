---
description: A multi-file edit script checks all its anchors in memory before writing the first file
trigger: always_on
---

# RULE: A multi-file edit script validates all its anchors before writing the first file

## Rule

1. **Two phases, never interleaved**: load all files in memory and check all anchors (presence, exact number of occurrences), then write — only if every check passed.
2. **Measure a count before asserting it**: `grep -c '<anchor>' <file>` on the real file, then put that number in the script.
3. **If an edit script still fails**: `git status --short` first, to know exactly what was written, then replay only the rest. Never rerun the whole script on a partially modified tree.

### A loop written inline is an edit script too

1. **A quick script typed in a heredoc, over several files, is held to the same two phases**: a loop of the shape `for path, edits in plan.items(): …check…; write(path)` writes each file before the next one is checked. Build every new text in a dictionary first, check every anchor, and write only after the loop.
2. **When a batch tool exists in the session, use it** rather than writing the loop again: it already loads, checks, then writes.
3. **Anchors copied from code written before `clang-format` ran are stale**: read the formatted text (`sed -n`, `grep -n`) before writing an anchor for a file the formatter touched.

### A script that keeps some changes of a file decides line by line

1. **When a script keeps only some of the changes between two versions of a file** (one commit's lines of a shared index, one point of a rule), it decides for each changed line, never for each block a diff tool returns: `difflib` and `git diff` group adjacent added lines into one block, so two index lines meant for two commits are kept or dropped together.
2. **Its check counts the lines kept against the lines named**: each marker must match exactly one changed line, and the lines kept must be exactly the lines the markers name. A marker matched once inside a block of two lines passes a per-block check and still carries the other line.

### Two files are already a batch

1. **The two-phase shape holds from the second file on**: a loop `for path, pairs in edits.items(): check; write` writes the first file before the second is checked, however short the edit. Fill a dictionary of new texts in the loop, write after it.

## Example

- ❌ **Before (wrong)**: a script renaming `PacketMgr` in six files, writing as it goes, fails on the fourth on a guessed anchor → three files changed, three not, the build breaks.
- ✅ **After (right)**: `grep -c 'PacketMgr'` on each file → the script checks the six anchors in memory, then writes the six files at once; build and an empty `grep -rn PacketMgr src` to conclude.
- ❌ **Before (wrong)**: an inline Python loop over six files wrote each one after checking its anchors; the fifth anchor had been reflowed by `clang-format`, so four files were written and two were not.
- ✅ **After (right)**: the six edits go into a spec file run by the batch tool (`apply_edits.py`), which checks every anchor in memory before writing any file; the anchors are read from the formatted files.
- ❌ **Before (wrong)**: a staging script kept the `difflib` block holding the index line of the test-file rule, so the first commit of a rules pull request also carried the index line of the constants rule, added on the next line.
- ✅ **After (right)**: the script splits each block into its lines and keeps only the lines a marker names; the staged index shows one added line per commit.
