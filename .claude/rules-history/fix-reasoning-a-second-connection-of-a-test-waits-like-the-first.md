# RULE: The second connection of a test waits for a lock like every connection of the project

## Context

`DatabaseFileFixture::openWhileAnotherConnectionWrites()` opens a file with a connection `other` that begins a write transaction, starts `openDatabaseFile()` in a `std::async` thread, waits `WRITE_HELD_FOR`, then commits `other`.

## Mistake

`other` was built with `readWriteOrCreate()` only, so SQLite's busy timeout was 0. On the Windows job of the CI, the `COMMIT` of `other` met a read lock held by the opening and failed at once with "database is locked", thrown in the test body. Unwinding destroyed the `opening` future, which waits for its thread; that thread retried the switch to WAL for `DATABASE_BUSY_TIMEOUT_MILLISECONDS` (5 s) before giving up, so the test took 6.49 s. The test failed on `main` after #267 and on `feat/admin-console-bans`, and passed on other runs.

## Root cause

The connection of the test was given less than the project's connections have. A timing-dependent lock was therefore met by a connection that cannot wait, on the platform whose lock timing differs.

## Rule

See `.claude/rules/fix/reasoning/fix-reasoning-a-second-connection-of-a-test-waits-like-the-first.md`.
