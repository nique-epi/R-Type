---
description: In plan mode, verification commands write nothing; their output goes through a pipe
trigger: always_on
---

# RULE: In plan mode, a check writes no file — not even in the scratchpad

## Rule

While plan mode is active, the only file written is the plan. A check compiles from stdin (`-fsyntax-only -x c++ -`) and filters its output through a pipe (`2>&1 >/dev/null | grep …`), never `> file`. A check that needs a file (a binary to run, clang-tidy on a path) waits for the plan to be approved and is listed in the plan's verification.

### A part of the plan is drafted in the plan, never beside it

1. **A section being rewritten is written straight into the plan file**: an edit of the plan in place, or a script that builds the text in memory and writes the plan path only. A draft file next to it (`scratchpad/plan_section_4.md`) is a written file like any other, whatever its name or folder.
2. **Before running a script in plan mode, read every path it opens for writing**: the plan path is the only one allowed. Code checked with `clang-format` goes through standard input and comes back on standard output, into the plan.

## Example

- ❌ **Before (wrong)**: `clang++ … 2> scratchpad/proxy.log` during plan mode.
- ✅ **After (right)**: `clang++ … 2>&1 >/dev/null | grep -E '<stdin>:[0-9]+:[0-9]+: warning'`.
- ❌ **Before (wrong)**: the ScoreBoard section of the plan written to `scratchpad/plan_section_4.md`, then spliced into the plan by a script.
- ✅ **After (right)**: the script builds the section in memory, formats its code through `clang-format` on standard input, and writes the plan file only.
