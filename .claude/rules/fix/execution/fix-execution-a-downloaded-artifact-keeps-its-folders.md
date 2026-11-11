---
description: A file uploaded from a sub-folder of an artifact comes back in that sub-folder, so a step that hands a glob of the download folder to a release gets a folder
trigger: always_on
---

# RULE: A downloaded artifact keeps its folders — a release is given its files, never a glob of the folder

Why: the package workflow uploaded `build/velopack/*` with the packages of `build/`; `actions/download-artifact` gave them back in `packages/velopack/`, and `gh release create "$GITHUB_REF_NAME" packages/*` stopped on `read packages/velopack: is a directory`, after every package job of v1.1.0 had passed.

## Rule

1. **Before adding a path to an `actions/upload-artifact` step, read where the step that downloads it hands the files on.** The artifact keeps each path relative to the deepest folder all its paths share: `build/R-Type-*.exe` and `build/velopack/*.json` come back as `R-Type-*.exe` and `velopack/*.json`.
2. **A step that publishes the files of a download folder lists its files**, never a glob of the folder: `find packages -type f | sort` read into an array with a `while read` loop (`mapfile` is missing from the bash 3.2 of macOS, where the step is tried). It stops when two files share a name, since an asset of a release is named after its file only.
3. **Try the step on a real artifact before the push**: download it (`gh run download <run> -n <artifact>`), copy its folder where the step reads it, put a `gh` that prints its arguments first on `PATH`, and run the `run:` text of the step, read from the workflow, with `bash -e`. Check its arguments and its stop on a repeated name.

## Example

- ❌ **Before (wrong)**: `gh release create "$GITHUB_REF_NAME" packages/*` once the client packages gained `build/velopack/` → `read packages/velopack: is a directory`, no release.
- ✅ **After (right)**: the files of `packages/` read with `find … -type f` into an array, a stop on a repeated name, then `gh release create "$GITHUB_REF_NAME" "${files[@]}"`, tried on the Linux artifact of the failed run.
