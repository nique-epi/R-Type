---
description: "Every cmake --build passes --parallel: the Unix Makefiles generator builds on one core otherwise, and a long verification is timed and announced"
trigger: always_on
---

# RULE: Every `cmake --build` runs in parallel, and a long verification is timed before it is run in full

## Context

On the branch of the launch intro (25 October 2026), the user asked for two changes to the intro while the gates ran. The work was built and tested in an isolated copy of the repository, configured with the Unix Makefiles generator, after each sync of the tree.

## Mistake

Every `cmake --build` of the copy ran without `--parallel`, so make built one file at a time on a 12-core machine, about ten times over. The red-proof runner had crashed at its fourth variant (an object file deleted twice) and was not rerun. The user saw about 50 minutes pass and said it was not normal.

## Root cause

The default of the generator was assumed to be parallel, as Ninja is; it was never read in `CMakeCache.txt`. The duration of the verifications was never measured nor announced, so nothing showed that they were slow.

## Rule

1. Every `cmake --build` passes `--parallel "$(sysctl -n hw.ncpu)"` (macOS) or `--parallel "$(nproc)"` (Linux), the builds of an isolated copy and of scripts included; `ctest` takes `-j`. The workflow presets are left as configured.
2. Read `CMAKE_GENERATOR` in the cache before relying on a default.
3. A verification made of repeated steps is timed on its first step, announced with its estimate, and run in the background.
4. A script that stops on an error is rerun as soon as the error is fixed.

## Example

- ❌ **Before (wrong)**: ten serial builds of the copy, a crashed red-proof run left aside, an hour gone.
- ✅ **After (right)**: parallel builds, the first variant timed, the run announced and run in the background, rerun at once after its fix.
