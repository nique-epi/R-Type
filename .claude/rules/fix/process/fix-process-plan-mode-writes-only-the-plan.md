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

### The script itself is a file

1. **In plan mode, a helper script is never saved with the write tool**, not even in the scratchpad: one that searches a transcript, renames a word across the plan or counts anchors runs inline, `python3 -I - <args> <<'PY'`, its paths given as arguments.
2. **Before every call to the write tool in plan mode, compare its path with the plan path**: any other path is refused by this rule, whatever the file is for.

### A read is never redirected into a file

1. **In plan mode, a read goes to standard output**: `git show`, `sed -n`, `cat`, a search or a listing prints, never `> file`, the scratchpad included. A redirection writes a file, whatever the command reads.
2. **A read too long for one output is cut into ranges** (`sed -n '1,200p'`, then `'201,400p'`), never saved to read later.

## Example

- ❌ **Before (wrong)**: `clang++ … 2> scratchpad/proxy.log` during plan mode.
- ✅ **After (right)**: `clang++ … 2>&1 >/dev/null | grep -E '<stdin>:[0-9]+:[0-9]+: warning'`.
- ❌ **Before (wrong)**: the ScoreBoard section of the plan written to `scratchpad/plan_section_4.md`, then spliced into the plan by a script.
- ✅ **After (right)**: the script builds the section in memory, formats its code through `clang-format` on standard input, and writes the plan file only.
- ❌ **Before (wrong)**: `scratchpad/tools/find_thinking.py` and `scratchpad/tools/rename_label.py` saved with the write tool during plan mode, to search the transcript and to rename a component across the plan.
- ✅ **After (right)**: both run inline, `python3 -I - <transcript> <keyword> <<'PY'` printing to standard output, and `python3 -I - <plan> <<'PY'` writing the plan path only.
- ❌ **Before (wrong)**: `git show <commit>:<rule> > <scratchpad>/code-style-rules.txt` during plan mode, to read the code style rules in parts.
- ✅ **After (right)**: `git show <commit>:<rule> | sed -n '1,200p'`, then the next range.
