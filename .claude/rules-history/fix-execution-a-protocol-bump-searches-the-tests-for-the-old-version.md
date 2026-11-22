# RULE: A change of `PROTOCOL_VERSION` searches the tests for the old version in every form it is written

## Context

The fosse level gained its boss and its transition; the protocol went to 18 and the boss sheets were rebuilt at half size with every frame.

## Mistake

the protocol went from 17 to 18, the test constants named after the version followed, but the default header bytes (`0x11`), a refusal text (`V17, SERVER V18`) and `SOME_OTHER_VERSION = 18` stayed; seven tests failed, and one "other version" became the version of the build.

## Root cause

The change was checked by the targets it built and a server run, not by the tests that read the old values or the client reader.

## Rule

1. **In the commit that changes `PROTOCOL_VERSION`, search the tests for the old number in each form a test writes it**:
   ```bash
   git grep -n -P '\b0x11, 0x00\b|\bV17\b|version 17|VERSION = 18;' -- tests
   ```
   with the old version in hexadecimal, as `V<n>` and in prose, and the new version as a literal constant (`-P`: `git grep -E` ignores `\b`). Read each hit: a header byte, a refusal text and a comment follow the new version; a byte that only shares the value (the actions of a `PlayerInput`) stays.
   Calibration: run with `28353565` in place of `--`, it prints the header bytes of `ProtocolTestConstants.hpp:116` and `DatagramCodecTest.cpp:107`, the text of `ConnectionScreenTest.cpp:448` and `SOME_OTHER_VERSION = 18`.
2. **A test constant meant to be another version is derived from the version of the build**, `PROTOCOL_VERSION + 1`, never written as a number: the next bump makes a literal the version of the build.
3. **Run the network and connection suites of that commit before it is called done**: `ctest --test-dir build -R 'DatagramCodec|ConnectionScreen|GuaranteedDelivery'`.

## Example

- ❌ **Before (wrong)**: `PROTOCOL_VERSION = 18` with `DEFAULT_HEADER_BYTES` still holding `0x11` and `SOME_OTHER_VERSION = 18` → `DatagramCodec.*` and `AVersionRefusalNamesBothVersions` red.
- ✅ **After (right)**: the search of point 1 lists the three, the header holds `0x12`, the text `V18, SERVER V19`, and `SOME_OTHER_VERSION = PROTOCOL_VERSION + 1`.

## Update (2026-11-21) — a constant that holds the old version as a number

While `LevelStatus` gained the continues left of a team, the protocol went from 19 to 20. The search of point 1 was run with the header bytes, `V19` and `version 19`; a wider search then showed `PROTOCOL_PAGE_VERSION = 19`, which was changed, and `ANNOUNCED_PROTOCOL_VERSION = 19` of the master message tests, which was judged a sample value without reading the assertion that compares it with the registration the server builds from `PROTOCOL_VERSION`. The full suite failed on `MasterMessages.TheRegistrationTellsWhatTheServerIs` (20 against 19), before any commit.

Root cause: the pattern of point 1 holds the new version as a literal constant only, and a hit was dismissed from its name instead of from the assertion that reads it.

Added to the rule: the search for the old version as a literal constant, with its calibration on `1d156870`, the reading of the assertion before a hit is left, and the master suite among the suites run.

