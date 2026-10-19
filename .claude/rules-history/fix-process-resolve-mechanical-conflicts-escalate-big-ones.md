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
