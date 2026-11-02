---
description: "A function has at most 35 lines, counted on the file as it stands after the change: a long function of a file the change touches is split in the same pull request"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: A function has at most 35 lines — counted after the change, not before

Why: a limit read on the diff leaves every function that was already long alone, so nobody ever splits it. Counted on the result, the function of a file the change touches is split by the change that touches the file.

## Rule

1. **Every function, constructor and lambda that has a name of its own in a `.cpp` or `.hpp` the change adds or changes is at most 35 lines**, from its signature to its closing brace. Test bodies (`TEST`, `TEST_F`, `TEST_P`) count like any function.
2. **The count is taken on the file after the change.** "It was already that long" is not an exemption: a function over the limit in a file the change touches is split in the same pull request.
   - Contained impact (the file or its module, same behavior, proved by the build and the tests): a `refactor` commit of its own (`fix-process-rework-preexisting-inconsistencies.md`).
   - High impact: ask first, as that rule says.
   - A file the change does not touch is reported, not reworked.
3. **Split along the steps of the body** (`fix-architecture-a-function-does-one-kind-of-step.md`), never by cutting it in two arbitrary halves:
   - a step repeated several times with other values becomes one helper called several times (`countedSince(current, reported)` for four counters);
   - a list of registrations or options becomes one function per entry and a short function that lists them (`addressOption(values)`, `portOption(values)`, then `read()` listing them);
   - checks, writes and notifications go in separate functions.
   A helper that is only used inside one `.cpp` goes in its anonymous namespace; a new member function counts towards `fix-architecture-at-most-ten-methods-per-class.md`.
4. **Measure every `.cpp` the change adds or changes, before the push**, with the SDK flags of `fix-execution-run-clang-tidy-before-push.md`:
   ```bash
   clang-tidy -p build --quiet --config='{Checks: "-*,readability-function-size", CheckOptions: {readability-function-size.LineThreshold: 35}}' <changed .cpp files>
   ```
   - The check reports each function over the limit. It must print nothing for the files of the change.
   - Calibration: on `ServerApplication.cpp` as `origin/main` has it, plus the `listenOn()` the change added, it reports `reportDiscardedDatagrams`, 41 lines; after the split, 35 lines, it reports nothing.
5. **`.clang-tidy` does not set this threshold**, and setting it would fail the CI on every long function of the repository at once: this rule is what guards it, as for the parameters (`fix-architecture-at-most-three-parameters.md`).

## Example

- ❌ **Before (wrong)**: a change adds `listenOn()` to `ServerApplication.cpp`; the file holds `reportDiscardedDatagrams()`, 41 lines that repeat the same count, compare and remember step four times; the diff does not touch it, so it stays.
- ✅ **After (right)**: the same pull request adds a `refactor` commit with `countedSince()` and the four reports as four short blocks; the function is 35 lines, and the measure of point 4 prints nothing.
- ❌ **Before (wrong)**: `LaunchOptions::read()` grows to 65 lines, one lambda per option inside one array.
- ✅ **After (right)**: one small function per option and a `read()` of 15 lines that lists them.
