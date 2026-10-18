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
