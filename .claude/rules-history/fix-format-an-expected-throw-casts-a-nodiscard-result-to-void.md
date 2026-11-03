# RULE: `EXPECT_THROW` around a `[[nodiscard]]` call casts the result to `void`

## Context

The traffic counters add `--traffic-csv` to the launch options of both programs, with a test that the option without a value is refused.

## Mistake

The test was written as `EXPECT_THROW(LaunchOptions::read(Arguments{TRAFFIC_CSV_OPTION}), MissingLaunchOptionValueException);`. It passed, and the build log held `ignoring return value of function declared with 'nodiscard' attribute`, which warnings as errors turn into a failure on the CI. The plan had shown the same line, and its check by compiling from standard input did not cover this file.

## Root cause

`LaunchOptions::read()` is `[[nodiscard]]`, and `EXPECT_THROW` discards the value of its statement. Every other `EXPECT_THROW` of the file already wrote `static_cast<void>(…)`; the new test was written without reading them.

## Rule

1. **Before writing `EXPECT_THROW` or `ASSERT_THROW`, read the declaration of the call inside it.** A call marked `[[nodiscard]]` goes through `static_cast<void>(…)`.
2. **A constructor that throws needs no cast**: name the object, `EXPECT_THROW(const TrafficCsvFile opened{path}, …)`.
3. **Read the build log of the test before its result**: a test that passes can still hold the warning that fails the CI.

## Example

- ❌ **Before (wrong)**: `EXPECT_THROW(LaunchOptions::read(Arguments{TRAFFIC_CSV_OPTION}), MissingLaunchOptionValueException);` → `ignoring return value of function declared with 'nodiscard' attribute`.
- ✅ **After (right)**: `EXPECT_THROW(static_cast<void>(LaunchOptions::read(Arguments{TRAFFIC_CSV_OPTION})), MissingLaunchOptionValueException);`.
