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
4. **Measure every `.cpp` the change adds or changes, before the push**, with the SDK flags of `fix-execution-run-clang-tidy-before-push.md`, at the two thresholds the subsection below explains:
   ```bash
   clang-tidy -p build --quiet --config='{Checks: "-*,readability-function-size", CheckOptions: {readability-function-size.LineThreshold: 34}}' <changed .cpp files>
   clang-tidy -p build --quiet --config='{Checks: "-*,readability-function-size", CheckOptions: {readability-function-size.LineThreshold: 20}}' <changed .cpp files>
   ```
   - The run at 34 must print nothing for the files of the change.
   - Each function the run at 20 reports whose opening brace is not on its first line is counted by hand, as the subsection below says.
   - Calibration, measured on `main` at `dc2eced`, both counts named:
     - `ConnectionAcceptor::answer` (`src/server/Lobby/ConnectionAcceptor/ConnectionAcceptor.cpp`, lines 122 to 159, opening brace on line 124): 38 lines by point 1, 35 by the check. The run at 34 reports it; a threshold of 35 lets it through.
     - `sortReadyStateThenRequest` (`tests/server/Application/ServerApplicationTest.cpp`, lines 73 to 109, opening brace on line 75): 37 lines by point 1, 34 by the check. The run at 34 prints nothing; the run at 20 reports 34, and 34 + 1 + 2 = 37: over the limit.
     - `paintPolaroid` (`src/client/Widgets/FlightLog/MissionLogPage/MissionLogPage.cpp`, lines 161 to 195, opening brace on line 162): 35 lines by point 1, 33 by the check, and 33 + 1 + 1 = 35: at the limit, not over it.
5. **`.clang-tidy` does not set this threshold**, and setting it would fail the CI on every long function of the repository at once: this rule is what guards it, as for the parameters (`fix-architecture-at-most-three-parameters.md`).

### The check counts from the opening brace, not from the signature

1. **`readability-function-size` counts the line of the body's closing brace minus the line of its opening brace**, and reports a function only when that count is above the threshold (`FunctionSizeCheck.cpp` of clang-tidy 20.1.8). Point 1 counts from the first line of the signature to the closing brace, both included. So point 1's count is the check's count, plus 1, plus the lines of the signature above the opening brace.
2. **Hence the threshold of 34**: a function whose opening brace ends its first line is 36 lines by point 1 when the check counts 35, which a threshold of 35 does not report. At 34, the check reports such a function exactly when it is over 35 lines.
3. **A signature that runs over several lines** (wrapped parameters, a constructor's initializer list) is left out of the check's count, so the run at 34 can stay silent on a function over the limit. For each function the run at 20 reports whose opening brace is below its first line, compute `<check's count> + 1 + (<brace line> - <first line>)`: it must be at most 35. The run at 20 sees a signature of up to 14 lines above its brace (34 − 20); a longer one is counted by hand.
4. **The check never counts a lambda on its own**: a lambda inside a function counts in that function's lines, and a named lambda outside any function (`const auto name = [](…) {…};` in a namespace) is counted by hand, from its first line to its closing brace.

## Example

- ❌ **Before (wrong)**: a change adds `listenOn()` to `ServerApplication.cpp`; the file holds `reportDiscardedDatagrams()`, 41 lines that repeat the same count, compare and remember step four times; the diff does not touch it, so it stays.
- ✅ **After (right)**: the same pull request adds a `refactor` commit with `countedSince()` and the four reports as four short blocks; the function is 35 lines (34 by the check), and the run at 34 of point 4 prints nothing.
- ❌ **Before (wrong)**: `LaunchOptions::read()` grows to 65 lines, one lambda per option inside one array.
- ✅ **After (right)**: one small function per option and a `read()` of 15 lines that lists them.
- ❌ **Before (wrong)**: the server's `LaunchOptions::usage()`, lines 123 to 158, 36 lines by point 1, passed the measure at `LineThreshold: 35`, since the check counted 35; a review bot found it on the pull request.
- ✅ **After (right)**: the run at 34 reports it (35 by the check, 36 by point 1), and it is split before the push.
