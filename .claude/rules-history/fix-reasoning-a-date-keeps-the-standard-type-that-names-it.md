# History: a date keeps the `std::chrono` type that names it

## Context

The leaderboard of a server writes, for each finished game, the moment it ended into the SQLite file of the server (`played_at`), and sends it to the players in `LeaderboardAnswer` (`playedAt`, an `i64` of seconds since 1970 UTC). The user had asked not to reinvent what already exists.

## Mistake

`GameResultKeeper.cpp` held a helper written by hand, `std::int64_t secondsSince1970()`, built from `duration_cast<seconds>(system_clock::now().time_since_epoch()).count()`, and `GameResult::endedAt` was an `std::int64_t` whose unit was only in its comment. A reviewer asked on the helper why it used neither a `using` nor a constant.

## Root cause

The value was written in the shape of the wire and of the file from the moment it was taken, so the integer travelled through the server; `<chrono>` was not searched for the name of "seconds since 1970", which C++20 gives as `std::chrono::sys_seconds`, an alias of the standard library.

## Rule

A date is a `std::chrono::sys_time<Duration>` (`sys_seconds` to the second) and a duration a `std::chrono::duration`; the count leaves the type only at the boundary that needs an integer (a bound SQL parameter, a field of a network message, a journal line); before writing a helper that computes a time value, look for its name in `<chrono>`; tests build their dates in the same type.

## Example

- ❌ **Before (wrong)**: `std::int64_t secondsSince1970()`, and `GameResult::endedAt` an `std::int64_t`.
- ✅ **After (right)**: `GameResult::endedAt` a `std::chrono::sys_seconds` set with `floor<seconds>(system_clock::now())`; the count is taken only when `ResultsDatabase` binds `:played_at`.
