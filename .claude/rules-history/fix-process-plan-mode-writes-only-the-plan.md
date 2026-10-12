# RULE: In plan mode, a check writes no file — not even in the scratchpad

## Context

While planning the engine event bus in plan mode, the code shown in the plan was checked before the plan was written: compiled from stdin with `-fsyntax-only`, then passed through a Clang proxy for MSVC conversion warnings.

## Mistake

The proxy's output was redirected to `scratchpad/proxy.log` to be filtered afterwards. Plan mode allows writing the plan file only; every other action must be read-only.

## Root cause

The scratchpad felt like private scratch space, outside the project, so writing there did not register as a change. The habit of capturing output in a file before filtering it (right outside plan mode, see `fix-execution-zsh-pipestatus.md`) was applied without checking the mode.

## Rule

While plan mode is active, the only file written is the plan. A check compiles from stdin and filters its output through a pipe, never into a file. A check that needs a file waits for the plan to be approved and is listed in the plan's verification.

## Example

- ❌ **Before (wrong)**: `clang++ … 2> scratchpad/proxy.log` during plan mode.
- ✅ **After (right)**: `clang++ … 2>&1 >/dev/null | grep -E '<stdin>:[0-9]+:[0-9]+: warning'`.
