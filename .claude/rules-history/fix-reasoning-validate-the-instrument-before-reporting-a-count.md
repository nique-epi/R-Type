# History: fix-reasoning-validate-the-instrument-before-reporting-a-count

The rule came from another project when R-Type started; its original incident is not recorded here.

## Update (2026-10-15) — a test parser blind to parameterized names

### Context
Connection screen branch: 19 red proofs on an isolated copy, run by a script that read the CTest log and listed the failed tests with the pattern `^\s*\d+ - (\S+) \(Failed\)`.

### Mistake
Variant 11 (two refusal lines swapped) came back with no failed test. The raw log showed `Test #715: EveryReason/RefusalTest.ARefusalShowsItsReason/(1-byte object <01>, 0x105434156 pointing to "REFUSED: GAME FULL") ***Failed`: the name holds spaces, so `\S+` never matched it. The same log showed a memory address in the name, which changes at every build, because the parameter tuple held a `const char*`.

### Root cause
The parser was tried on no failing test before being trusted, and the only parameterized suite of the run was the one whose names it could not read.

### What was done
The verdict of variant 11 was read from the raw log (red). The tuple of the test now holds a `std::string_view`, printed as text. The rule gained the subsection on parsers of test results.

## Update (23 October 2026) — a rule's grep calibrated with another tool

The rule `fix-format-a-constants-header-ends-in-constants.md`, written that day after Yannis Lorang's review of pull request #103, gave two checks as `git grep -n -E` with `\b` in the pattern, calibrated by piping `git show` into plain `grep -E`. A test agent ran the rule as written: on git 2.53, `git grep -E` ignores `\b`, so the first check found 0 constants in a header holding 36, and the second missed `struct Burst` and `struct Ring`. Measured: `git grep -E` 0, `git grep -P` 36, `grep -E` 36. The checks now use `-P`, and their calibration is written as the same `git grep` command run on a commit.

## Update (24 October 2026) — A filter of diff lines keeps every kind of line it must show

While staging the first commit of a rules pull request, the staged index was read through `git diff --cached -U0 | grep -E '^[-+][^-+]'`, meant to drop the `+++` and `---` headers. It also dropped every added Markdown bullet (`+- …`): only the changed table row printed, and the stray index line of the next commit's rule went unseen until a second read of the file itself. Nothing was committed with it. New subsection: drop only the two headers (`grep -v -E '^(\+\+\+|---) '`), and check the filter shows a line of each kind the change holds.
