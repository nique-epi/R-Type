---
description: "A date or a duration keeps the std::chrono type that names it; its count of seconds or milliseconds appears only where it leaves the program"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: A date keeps the `std::chrono` type that names it; its count appears only where it leaves the program

Why: a review asked why a hand-written `secondsSince1970()` returned a bare `std::int64_t`, while C++20 already names a date to the second, `std::chrono::sys_seconds`.

## Rule

1. **A date is a `std::chrono::sys_time<Duration>`**, `std::chrono::sys_seconds` to the second, taken with `std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now())`. A duration is a `std::chrono::duration` (`engine::Duration`, `std::chrono::milliseconds`). Never an integer whose unit lives only in a name (`secondsSince1970`, `delayMs`) or a comment.
2. **The count leaves the type only at the boundary that needs an integer**: the bound parameter of a SQL statement, a field of a network message, a line of the journal. `static_cast<std::int64_t>(date.time_since_epoch().count())` is written at that call and nowhere else.
3. **Before writing a helper that computes a time value, look for its name in `<chrono>`**: since C++20 the epoch of `system_clock` is 1970 UTC, so "seconds since 1970" is `sys_seconds`.
4. **A test builds its dates in the same type**: `constexpr std::chrono::sys_seconds SOME_DATE{std::chrono::seconds{1'800'000'000}};`, and a later one as `SOME_DATE + std::chrono::minutes{1}`.

## Example

- ❌ **Before (wrong)**: `std::int64_t secondsSince1970()` built from `duration_cast<seconds>(system_clock::now().time_since_epoch()).count()`, and `GameResult::endedAt` an `std::int64_t` with "in seconds since 1970 UTC" in its comment.
- ✅ **After (right)**: `GameResult::endedAt` is a `std::chrono::sys_seconds`, set with `floor<seconds>(system_clock::now())`; `ResultsDatabase` binds `endedAt.time_since_epoch().count()` to `:played_at`, and `LeaderboardRow::playedAt` stays the `i64` of the wire.
