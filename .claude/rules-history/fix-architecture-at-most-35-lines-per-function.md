# RULE: A function has at most 35 lines — counted after the change, not before

## Context

Pull request of the command interpreter and the configuration files. After the work was applied, the user asked for the functions written or changed to stay around 35 lines. A measure with `readability-function-size` and `LineThreshold: 35` on the 42 `.cpp` the branch changed found one function over the limit, `ServerApplication::reportDiscardedDatagrams`, 41 lines, which the branch had not written: it only added `listenOn()` to the same file. Before that, `LaunchOptions::read()` of the client had grown from 48 to 65 lines.

## Mistake

The first reading counted the functions the diff touched, so `reportDiscardedDatagrams` was reported as "already there" and left alone. The user corrected it: the functions are counted after the changes, not before, otherwise nobody ever refactors them.

## Root cause

No rule set a length for a function. `a-function-does-one-kind-of-step` only names a body longer than 15 lines as a signal, and `rework-preexisting-inconsistencies` was read as covering conventions, not size. A function that was already long in a file the change touches fell between the two.

## Rule

1. Every function, constructor and lambda with a name of its own in a `.cpp` or `.hpp` the change adds or changes is at most 35 lines, signature to closing brace, test bodies included.
2. The count is taken on the file after the change; a function over the limit in a touched file is split in the same pull request, in a `refactor` commit of its own when the impact is contained.
3. The split follows the steps of the body: one helper for a repeated step, one function per entry of a list of options, checks and writes apart.
4. Measured before the push with `clang-tidy` and `readability-function-size.LineThreshold: 35` on the changed `.cpp`.
5. `.clang-tidy` does not set the threshold; the rule guards it.

## Example

- ❌ **Before (wrong)**: `reportDiscardedDatagrams()`, 41 lines, left as it was because the diff only added `listenOn()` to its file.
- ✅ **After (right)**: `countedSince()` takes the repeated step, the function is 35 lines, and the measure prints nothing.

## Update (2026-11-02) — The measure counted one line less than the rule

### Context

Network conditions pull request (#166). While its review comments were resolved, a review bot reported two `usage()` functions of 36 lines, among them the server's `LaunchOptions::usage()`, lines 123 to 158 at `ed4b823`. The measure of point 4, run with `LineThreshold: 35` before the push, had printed nothing for them.

### Mistake

The threshold of the command was copied from point 1 without checking how `readability-function-size` counts. At `ed4b823`, `LaunchOptions::read` spans lines 80 to 121, 42 lines by point 1, and the check's note said 41 lines; `usage()` was not reported at 35 and was reported at 34. The calibration of the rule, a function of 41 lines and then of 35, sat away from the limit and could not show the gap.

### Root cause

The two counts were never compared at the bound: no calibration case was 35 or 36 lines (`fix-reasoning-bounded-contract-check-the-empty-state.md`). `FunctionSizeCheck.cpp` of clang-tidy 20.1.8, read afterwards, counts the line of the body's closing brace minus the line of its opening brace and reports only above the threshold. Two more gaps follow from it:

- the lines of a signature above the opening brace are not counted at all;
- a lambda is never counted on its own: the matcher leaves out the call operator of every lambda.

A standalone file run through clang-tidy 20.1.8 confirmed the first: at 35 nothing is reported; at 34 the 36-line function is reported (35 by the check), and neither the 35-line one (34) nor a 37-line one whose brace is two lines below its first line (34) is. On `main` at `dc2eced`, measured on the 733 `.cpp` of `src/` and `tests/` that compile on macOS (the 5 Windows-only files do not), 13 functions are over 35 lines by point 1. A threshold of 35 misses two of them: `ConnectionAcceptor::answer` (38 lines, 35 by the check) and `sortReadyStateThenRequest` (37 lines, 34 by the check, signature on three lines). A threshold of 34 alone still misses the second. Of the 89 functions the check counts above 20 lines, 36 have their opening brace below their first line, up to 7 lines below for the constructor of `GameSessions` and its initializer list. No named lambda sits outside a function on `main`.

### Rule (added)

- Point 4 runs the check at 34, which must print nothing, and at 20; each function the run at 20 reports with its opening brace below its first line is counted by hand: the check's count + 1 + the lines above the brace, at most 35.
- `### The check counts from the opening brace, not from the signature` states the relation between the two counts, and that a named lambda outside any function is counted by hand.
- The calibration is restated on `main` at `dc2eced` with both counts: `ConnectionAcceptor::answer` (38 and 35), `sortReadyStateThenRequest` (37 and 34), `paintPolaroid` (35 and 33).

### Example (added)

- ❌ **Before (wrong)**: the server's `LaunchOptions::usage()`, 36 lines by point 1, passed the measure at `LineThreshold: 35`, since the check counted 35.
- ✅ **After (right)**: the run at 34 reports it (35 by the check, 36 by point 1), and it is split before the push.
