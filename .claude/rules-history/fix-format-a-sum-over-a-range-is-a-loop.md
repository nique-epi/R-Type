---
description: "A sum over a range is a range-for loop: std::accumulate on two iterators fails boost-use-ranges, and C++20 has no std::ranges version of it"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: A sum over a range is a range-for loop, never `std::accumulate` on two iterators

## Context

`TickDurations` averages the durations of the ticks of a report, kept in a `std::span`.

## Mistake

The plan and the code summed them with `std::accumulate(sorted.begin(), sorted.end(), engine::Duration::zero())`. The first local `clang-tidy` run failed on `boost-use-ranges` with `use a boost version of this algorithm`. `git grep 'std::accumulate' -- src tests` printed nothing else: the repository has no such call.

## Root cause

The algorithm was chosen from habit, without searching the repository for how it sums a range. C++20 has no `std::ranges::accumulate`, so the check has no standard form to accept.

## Rule

1. **To add up a range, write a range-for loop** over a local total.
2. **Any other standard algorithm that takes two iterators goes through its `std::ranges` form** when C++20 has one (`std::ranges::sort`, `std::ranges::find`, `std::ranges::count_if`).
3. **Never add a Boost dependency to satisfy this check**: the server and the client use no Boost.

## Example

- ❌ **Before (wrong)**: `const Duration total = std::accumulate(sorted.begin(), sorted.end(), Duration::zero());` → `use a boost version of this algorithm`.
- ✅ **After (right)**: `Duration total = Duration::zero();` then `for (const Duration duration : sorted) { total += duration; }`.
