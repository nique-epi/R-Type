---
description: In plan mode, verification commands write nothing; their output goes through a pipe
trigger: always_on
---

# RULE: In plan mode, a check writes no file — not even in the scratchpad

## Rule

While plan mode is active, the only file written is the plan. A check compiles from stdin (`-fsyntax-only -x c++ -`) and filters its output through a pipe (`2>&1 >/dev/null | grep …`), never `> file`. A check that needs a file (a binary to run, clang-tidy on a path) waits for the plan to be approved and is listed in the plan's verification.

## Example

- ❌ **Before (wrong)**: `clang++ … 2> scratchpad/proxy.log` during plan mode.
- ✅ **After (right)**: `clang++ … 2>&1 >/dev/null | grep -E '<stdin>:[0-9]+:[0-9]+: warning'`.
