---
description: A change of PROTOCOL_VERSION searches the tests for every byte, text and constant of the old version
trigger: always_on
---

# RULE: A change of `PROTOCOL_VERSION` searches the tests for the old version in every form it is written

Why: the protocol went from 17 to 18, the test constants named after the version followed, but the default header bytes (`0x11`), a refusal text (`V17, SERVER V18`) and `SOME_OTHER_VERSION = 18` stayed; seven tests failed, and one "other version" became the version of the build.

## Rule

1. **In the commit that changes `PROTOCOL_VERSION`, search the tests for the old number in each form a test writes it**:
   ```bash
   git grep -n -P '\b0x11, 0x00\b|\bV17\b|version 17|VERSION = 18;' -- tests
   ```
   with the old version in hexadecimal, as `V<n>` and in prose, and the new version as a literal constant (`-P`: `git grep -E` ignores `\b`). Read each hit: a header byte, a refusal text and a comment follow the new version; a byte that only shares the value (the actions of a `PlayerInput`) stays.
   Calibration: run with `28353565` in place of `--`, it prints the header bytes of `ProtocolTestConstants.hpp:116` and `DatagramCodecTest.cpp:107`, the text of `ConnectionScreenTest.cpp:448` and `SOME_OTHER_VERSION = 18`.
2. **A test constant meant to be another version is derived from the version of the build**, `PROTOCOL_VERSION + 1`, never written as a number: the next bump makes a literal the version of the build.
3. **Run the network and connection suites of that commit before it is called done**: `ctest --test-dir build -R 'DatagramCodec|ConnectionScreen|GuaranteedDelivery'`.

### A constant that holds the old version as a number follows the bump too

1. **The search of point 1 also looks for the old version as a literal constant**: `git grep -n -P 'VERSION = 19;' -- tests`, with the old version. Its pattern only held the new version as a constant.
   Calibration: run with `1d156870` in place of `--`, it prints `PROTOCOL_PAGE_VERSION = 19` of `ReliabilityTestConstants.hpp:14` and `ANNOUNCED_PROTOCOL_VERSION = 19` of `MasterMessagesTestConstants.hpp:26`.
2. **A hit is never left as "a sample value" before the assertion that reads it is read**: `git grep -n '<NAME>' -- tests`. A constant compared with what the code builds from `PROTOCOL_VERSION` follows the bump.
3. **The suites of point 3 gain the master's**: `ctest --test-dir build -R 'DatagramCodec|ConnectionScreen|GuaranteedDelivery|MasterMessages'`.

## Example

- ❌ **Before (wrong)**: `PROTOCOL_VERSION = 18` with `DEFAULT_HEADER_BYTES` still holding `0x11` and `SOME_OTHER_VERSION = 18` → `DatagramCodec.*` and `AVersionRefusalNamesBothVersions` red.
- ✅ **After (right)**: the search of point 1 lists the three, the header holds `0x12`, the text `V18, SERVER V19`, and `SOME_OTHER_VERSION = PROTOCOL_VERSION + 1`.
- ❌ **Before (wrong)**: the protocol went from 19 to 20; `ANNOUNCED_PROTOCOL_VERSION = 19` was read as a sample value and left → `MasterMessages.TheRegistrationTellsWhatTheServerIs` red, the registration announcing 20.
- ✅ **After (right)**: the search for `VERSION = 19;` lists it with `PROTOCOL_PAGE_VERSION`, its assertion is read, and both hold 20.
