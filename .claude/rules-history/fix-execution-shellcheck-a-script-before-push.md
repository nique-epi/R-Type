---
description: A shell script the CI lints is run through shellcheck, with the CI's options, before it is pushed
trigger: always_on
---

# RULE: A shell script the CI lints goes through shellcheck before the push

## Context

The packaging pull request added `tests/tools/packaging/check-package.sh`, which the CI runs to check the packages from another folder, and extended the `shellcheck` step of the `Network condition scripts` job to cover `tests/tools/packaging/*.sh`.

## Mistake

The script launched the AppImage with `"$build"/R-Type-client-*.AppImage --help`. It was run on macOS, where that branch is not taken, and shellcheck was not installed locally; the job failed on `SC2211 (warning): This is a glob used as a command name`.

## Root cause

The gates run before the push covered C++ (build, tests, format, clang-tidy) but no step ran the linter the CI applies to shell scripts, and a local run of a script does not exercise what shellcheck checks.

## Rule

1. Before pushing a `.sh` that a CI step lints, run the command of that step on it, with the same options. Exit code 0, read without a pipe.
2. When shellcheck is not installed, run it from its image (`koalaman/shellcheck:v0.10.0`), Docker started first. If neither works, say the script is linted by the CI only.
3. A script that runs locally proves nothing to shellcheck.
4. Never run a command through a glob: resolve it into a variable first.

## Example

- ❌ **Before (wrong)**: `"$build"/R-Type-client-*.AppImage --help` pushed after a local run → `SC2211`, job red.
- ✅ **After (right)**: `appimage=$(echo "$build"/R-Type-client-*.AppImage)`, then `"$appimage" --help`, and the CI's shellcheck command run before the push.
