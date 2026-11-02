---
description: "A test reads a std::optional with value_or after ASSERT_TRUE: bugprone-unchecked-optional-access does not take ASSERT_TRUE for a check"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: A test reads a `std::optional` with `value_or`, even right after `ASSERT_TRUE`

Why: `bugprone-unchecked-optional-access` does not see `ASSERT_TRUE(report.has_value())` as a check, so `report->ticks` after it fails the `Clang-tidy` job.

## Rule

1. **After `ASSERT_TRUE(value.has_value())`, read the value with `value_or`**, never with `*value`, `value->` or `value.value()`: `const Position found = position.value_or(Position{});`, then assert on `found`. `WorldTest.cpp` and `EntitySpritesTest.cpp` do it this way.
2. **Keep the `ASSERT_TRUE`**: without it, a missing value reads the default and a wrong test can pass.
3. **Inside `if (value.has_value())`, `value->` stays allowed**: the check sees that branch.

## Example

- ❌ **Before (wrong)**: `ASSERT_TRUE(report.has_value());` then `EXPECT_EQ(report->ticks.count, TICKS_IN_TEN_SECONDS);` → `unchecked access to optional value` on each line.
- ✅ **After (right)**: `ASSERT_TRUE(report.has_value());` then `const TickDurationSummary ticks = report.value_or(MetricsReport{}).ticks;` and `EXPECT_EQ(ticks.count, TICKS_IN_TEN_SECONDS);`.
