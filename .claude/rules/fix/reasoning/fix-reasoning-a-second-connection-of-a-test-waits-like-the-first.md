---
description: A second database connection a test opens waits for a lock like the project's connections do, or its commit fails where the timing differs
trigger: always_on
---

# RULE: The second connection of a test waits for a lock like every connection of the project

Why: `WaitsForAnotherWriterBeforeSwitchingToWal` opened its writing connection with SQLite's default busy timeout of 0; on Windows the opening held a read lock when the test committed, the `COMMIT` failed with "database is locked", and the test then waited 5 s for the opening to give up (the destructor of a `std::async` future waits for its thread).

## Rule

1. **Every `SQLite::Database` a test opens beside the one under test** is given `DATABASE_BUSY_TIMEOUT_MILLISECONDS`, like `openDatabaseFile()` does. A connection with no timeout fails on the first lock it meets.
2. **A test that commits or writes while another thread opens the same file** walks both orders of the locks by hand: the commit before the opening reads, and the commit while it holds a read lock. The test passes in both, on Linux, macOS and Windows.
3. **A test that fails on one platform only, with "database is locked" after the busy timeout, is read for the connection that has no wait** before the code under test is changed.

## Example

- ❌ **Before (wrong)**: `SQLite::Database other{file(), readWriteOrCreate()};` then `other.exec("COMMIT")` while the opening reads the file → `database is locked` on the Windows job, and 6.5 s for a test that takes 0.2 s.
- ✅ **After (right)**: `SQLite::Database other{file(), readWriteOrCreate(), DATABASE_BUSY_TIMEOUT_MILLISECONDS};` → the commit retries until the opening lets go.
