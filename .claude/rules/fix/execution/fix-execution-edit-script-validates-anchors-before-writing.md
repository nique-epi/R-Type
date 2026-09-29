---
description: A multi-file edit script checks all its anchors in memory before writing the first file
trigger: always_on
---

# RULE: A multi-file edit script validates all its anchors before writing the first file

## Rule

1. **Two phases, never interleaved**: load all files in memory and check all anchors (presence, exact number of occurrences), then write — only if every check passed.
2. **Measure a count before asserting it**: `grep -c '<anchor>' <file>` on the real file, then put that number in the script.
3. **If an edit script still fails**: `git status --short` first, to know exactly what was written, then replay only the rest. Never rerun the whole script on a partially modified tree.

## Example

- ❌ **Before (wrong)**: a script renaming `PacketMgr` in six files, writing as it goes, fails on the fourth on a guessed anchor → three files changed, three not, the build breaks.
- ✅ **After (right)**: `grep -c 'PacketMgr'` on each file → the script checks the six anchors in memory, then writes the six files at once; build and an empty `grep -rn PacketMgr src` to conclude.
