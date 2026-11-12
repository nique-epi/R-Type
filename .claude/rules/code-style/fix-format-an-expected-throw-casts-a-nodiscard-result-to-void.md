---
description: "EXPECT_THROW around a [[nodiscard]] call casts its result to void: the macro ignores the result and the compiler warns"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: `EXPECT_THROW` around a `[[nodiscard]]` call casts the result to `void`

Why: `EXPECT_THROW(LaunchOptions::read(…), …)` discards what `read()` returns, which raises `-Wunused-result`, an error on the CI; the tests next to it already wrote the cast.

## Rule

1. **Before writing `EXPECT_THROW` or `ASSERT_THROW`, read the declaration of the call inside it.** A call marked `[[nodiscard]]` goes through `static_cast<void>(…)`.
2. **A constructor that throws needs no cast**: name the object, `EXPECT_THROW(const TrafficCsvFile opened{path}, …)`.
3. **Read the build log of the test before its result**: a test that passes can still hold the warning that fails the CI.

### A constructor of several arguments is named with parentheses

1. **The preprocessor splits a macro argument at every comma outside parentheses, braces do not protect it**: `EXPECT_THROW(const UniformGrid grid{TEST_AREA, NEGATIVE_CELL_SIDE}, …)` gives the macro three arguments and fails with `too many arguments provided to function-like macro invocation`.
2. **Name the object with parentheses when its constructor takes two arguments or more**: `EXPECT_THROW(const UniformGrid grid(TEST_AREA, NEGATIVE_CELL_SIDE), …)`. Braces stay for one argument only, as `opened{path}`.

## Example

- ❌ **Before (wrong)**: `EXPECT_THROW(LaunchOptions::read(Arguments{TRAFFIC_CSV_OPTION}), MissingLaunchOptionValueException);` → `ignoring return value of function declared with 'nodiscard' attribute`.
- ✅ **After (right)**: `EXPECT_THROW(static_cast<void>(LaunchOptions::read(Arguments{TRAFFIC_CSV_OPTION})), MissingLaunchOptionValueException);`.
