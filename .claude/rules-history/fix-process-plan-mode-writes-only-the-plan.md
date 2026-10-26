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

## Update (2026-10-22) — Helper scripts saved to the scratchpad during plan mode

**Context.** While planning the visual effects of the client (RTY-122) in plan mode, the context had been compacted: the design had to be found again in the transcript, and later a component name (`Label`, already taken by a widget) had to be renamed across an 8000-line plan.

**Mistake.** Two scripts were saved with the write tool in the scratchpad (`tools/find_thinking.py`, `tools/rename_label.py`) before being run. The second one was never run: the rename was done inline. Plan mode allows the plan file only, and the rule already said so twice.

**Root cause.** The rule was read as being about what a check or a script writes, not about the script file itself; a helper that only reads (searching the transcript) did not register as a write, although saving it is one.

**Addition to the rule.** In plan mode a helper script runs inline (`python3 -I - <args> <<'PY'`), never saved with the write tool; before every write in plan mode, its path is compared with the plan path.

## Update (2026-10-24) — a read redirected into the scratchpad

**Context.** While planning the launch intro and the sounds of the client (RTY-123) in plan mode, the code style rules had to be read again from a commit, a long output.

**Mistake.** The output was redirected into the scratchpad (`… > <scratchpad>/code-style-rules.txt`) to be read in parts. Plan mode allows the plan file only, and the rule already said so three times.

**Root cause.** The rule named checks, drafts and scripts; a plain read did not register as a write, although its redirection creates a file.

**Addition to the rule.** In plan mode a read prints to standard output; a long one is cut into ranges with `sed -n`, never saved to a file.
