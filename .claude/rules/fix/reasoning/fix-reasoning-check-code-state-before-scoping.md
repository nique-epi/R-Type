---
description: Check in the code that a feature does not already exist before classifying it as "to do"
trigger: always_on
---

# RULE: Check the state of the code before scoping

## Rule

Before classifying a feature as "to do" in a plan, an issue or a PR, **check in the code** that it is not already implemented — and on open branches (`gh pr list`) that a teammate is not already on it. Never infer progress from a mere phrasing by the user. When in doubt, raise it explicitly before scoping.

## Example

- ❌ **Before (wrong)**: planning "set up the CI" because the user mentioned the CI.
- ✅ **After (right)**: `gh pr list` → a `ci: build and test on Linux and Windows` PR is open → the CI is in progress, start from there.
