---
description: A shell script the CI lints is run through shellcheck, with the CI's options, before it is pushed
trigger: always_on
---

# RULE: A shell script the CI lints goes through shellcheck before the push

Why: `tests/tools/packaging/check-package.sh` ran an AppImage through a glob as its command name; it worked locally, and the `Network condition scripts` job failed on `SC2211`.

## Rule

1. **Before pushing a `.sh` that a CI step lints, run the command of that step on it**: read the step in `.github/workflows/` (today `shellcheck --shell=sh --external-sources <files>`) and run it with the same options. Exit code 0, read without a pipe.
2. **When shellcheck is not installed, run it from its image**: `docker run --rm -v "$PWD:/mnt" koalaman/shellcheck:v0.10.0 --shell=sh /mnt/<script>`, Docker started first. If neither works, say in the report that the script is linted by the CI only.
3. **A script that runs locally proves nothing to shellcheck**: a glob used as a command name, an unquoted variable or a `cd` without a check all run, and all fail the job.
4. **Never run a command through a glob**: resolve it into a variable first, `appimage=$(echo "$build"/R-Type-client-*.AppImage)`, then `"$appimage" --help`.

## Example

- ❌ **Before (wrong)**: `"$build"/R-Type-client-*.AppImage --help` pushed after a local run of the script → `SC2211 (warning): This is a glob used as a command name`, job red.
- ✅ **After (right)**: the variable form, and the CI's shellcheck command run on the script before the push.
