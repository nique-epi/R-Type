---
description: "Every cmake --build passes --parallel: the Unix Makefiles generator builds on one core otherwise, and a long verification is timed and announced"
trigger: always_on
---

# RULE: Every `cmake --build` runs in parallel, and a long verification is timed before it is run in full

Why: the build folders of this project use the Unix Makefiles generator, which builds one file at a time unless told otherwise; the user saw an hour go by on verifications that a 12-core machine runs in minutes.

## Rule

1. **Every `cmake --build` passes `--parallel`** (with no number, CMake lets the build tool pick, which for make means one job: write `--parallel "$(sysctl -n hw.ncpu)"` on macOS, `--parallel "$(nproc)"` on Linux). This includes the builds of an isolated copy and the builds a script runs (a red-proof runner, a per-commit check). `ctest` takes `-j` the same way. The workflow presets (`cmake --workflow --preset test`) are left as they are configured.
2. **Before reading a generator's default, read the cache**: `grep CMAKE_GENERATOR <build>/CMakeCache.txt`. Ninja builds in parallel by default, Unix Makefiles does not.
3. **A verification made of repeated steps (red-proof variants, one build per commit) is timed on its first step**, then announced to the user with its estimate before the rest runs, and run in the background while other work goes on.
4. **A script that stops on an error is rerun as soon as the error is fixed**, never left for later: a red-proof run that crashed at its fourth variant proves three variants, not twenty-one.

## Example

- ❌ **Before (wrong)**: ten `cmake --build "$S/isolated/repo/build"` after as many syncs, each serial; the red-proof runner crashed at variant 4 and was left; an hour passed.
- ✅ **After (right)**: `cmake --build "$S/isolated/repo/build" --parallel 12`; the first variant timed (a few seconds), the whole run announced, run in the background, rerun as soon as its crash was fixed.
