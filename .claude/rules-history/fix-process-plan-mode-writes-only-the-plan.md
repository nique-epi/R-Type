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

## Update (2026-10-21) — A draft of a plan section written beside the plan

**Context.** While planning the status band of the client (RTY-151) in plan mode, the ScoreBoard section of a 3000-line plan had to be rewritten.

**Mistake.** The new section was written to `scratchpad/plan_section_4.md`, then spliced into the plan by a script. Plan mode allows the plan file only.

**Root cause.** The rule was read as being about the output of checks; a draft of the plan itself did not register as "a file written", since its content was meant for the plan anyway.

**Addition to the rule.** A section being rewritten goes straight into the plan file, by an edit in place or a script that writes the plan path only; before running a script in plan mode, every path it opens for writing is read.
