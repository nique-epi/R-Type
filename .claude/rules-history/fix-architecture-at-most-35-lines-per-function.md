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
