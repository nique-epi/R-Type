# RULE: Resolve mechanical merge conflicts alone, NOTIFY as soon as a conflict is big or ambiguous

The rule was imported from another project when R-Type started; its original incident belongs to that repository. This file holds the incidents met on R-Type.

## Update (2026-10-19) — a union that dropped the end of a test

### Context
Merging `origin/main` into `feat/waiting-room` left a 413-line block in `tests/client/Connection/ServerConnectionTest.cpp`: 17 tests of the waiting room on one side, 9 tests of the player inputs on the other. The block was classified as an accumulation and resolved by keeping both sides.

### Mistake
Both sides started after the shared `/**` of their first test and ended before the shared `}` of their last one. Joining them as they were left the last test of the first side without its `}` and the first test of the second side without its `/**`. No marker was left; the build failed with "use of undeclared identifier 'Given'" and every later test became a local class. The same shape had been handled by hand in `ConnectionConstants.hpp`, in the same run.

### Root cause
The union was applied to the lines inside the block without reading the shared lines around it, and the result was judged by the absence of markers instead of a build.

### Rule
1. Before joining both sides of a block, read the line just above and just below it; when both sides start inside a shared opening or end before a shared closing, put the closing and the opening back between them.
2. A union is checked by compiling the file, never by the absence of markers.

### Example
- ❌ **Before (wrong)**: ours + theirs → `EXPECT_TRUE(…);` followed directly by ` * Given an attempt the server has not answered yet`.
- ✅ **After (right)**: ours + `}` + blank line + `/**` + theirs, then the file compiled.

## Update (2026-11-07) — a header included twice after a merge

### Context
Merging `origin/main` into `feat/client-chat` (pull request #268) resolved a conflict on the includes of `src/client/Game/GameScreen.cpp`.

### Mistake
The chat branch included `GameNoticeConstants.hpp` under `// NOLINTNEXTLINE(misc-include-cleaner)`, just above `GameScreenIdConstants.hpp`; `main` had added the same header in its sorted place. The merge kept both lines. The build and the 5249 tests passed; the CI `Clang-tidy` job failed on `readability-duplicate-include`, and the `NOLINTNEXTLINE` no longer covered `GameScreenIdConstants.hpp`, which it covers on `main`.

### Root cause
The resolution only compared the two sides of each block; the auto-merged lines of the same file, which held the other copy, were never searched.

### Rule
Look for each header of a joined include list in the whole file, check what a `NOLINTNEXTLINE` above the block now covers, and list repeated includes of every merged file before committing.

