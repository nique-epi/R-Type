---
description: "A sum over a range is a range-for loop: std::accumulate on two iterators fails boost-use-ranges, and C++20 has no std::ranges version of it"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: A sum over a range is a range-for loop, never `std::accumulate` on two iterators

Why: `boost-use-ranges` is on in `.clang-tidy` and reports `std::accumulate(first, last, …)`; `std::ranges::fold_left` is C++23, and the project is C++20.

## Rule

1. **To add up a range, write a range-for loop** over a local total.
2. **Any other standard algorithm that takes two iterators goes through its `std::ranges` form** when C++20 has one (`std::ranges::sort`, `std::ranges::find`, `std::ranges::count_if`).
3. **Never add a Boost dependency to satisfy this check**: the server and the client use no Boost.

## Example

- ❌ **Before (wrong)**: `const Duration total = std::accumulate(sorted.begin(), sorted.end(), Duration::zero());` → `use a boost version of this algorithm`.
- ✅ **After (right)**: `Duration total = Duration::zero();` then `for (const Duration duration : sorted) { total += duration; }`.
