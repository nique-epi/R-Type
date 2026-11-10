---
description: A file uploaded from a sub-folder of an artifact comes back in that sub-folder, so a step that hands a glob of the download folder to a release gets a folder
trigger: always_on
---

# RULE: A downloaded artifact keeps its folders — a release is given its files, never a glob of the folder

## Context

The client started updating itself through Velopack. Its packaging writes the feed of each system and the full package of an update into `build/velopack/`, and the package workflow added `build/velopack/R-Type-*-full.nupkg` and `build/velopack/releases.*.json` to the paths of its artifact, next to `build/R-Type-*`. The release workflow downloads every artifact into `packages/` and ran `gh release create "$GITHUB_REF_NAME" packages/*`.

## Mistake

The pull request ran the package jobs, which passed on the three systems, but never the publish job, which only runs on a tag. On the tag v1.1.0, the publish job stopped on `read packages/velopack: is a directory`: the artifact had kept `velopack/` as a folder, the glob handed that folder to `gh`, and no release was created.

## Root cause

The new paths were written for the upload alone; where the download puts them, and what the next step does with the folder, was never read.

## Rule

1. Before adding a path to an `actions/upload-artifact` step, read where the step that downloads it hands the files on. The artifact keeps each path relative to the deepest folder all its paths share.
2. A step that publishes the files of a download folder lists its files, never a glob of the folder, and stops when two share a name.
3. Try the step on a real artifact, with a `gh` that prints its arguments, before the push.

## Example

- Before: `gh release create "$GITHUB_REF_NAME" packages/*` → `read packages/velopack: is a directory`.
- After: the files read with `find packages -type f | sort` into an array, a stop on a repeated name, then `gh release create "$GITHUB_REF_NAME" "${files[@]}"`.
