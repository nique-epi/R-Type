---
description: In zsh, $VAR:something triggers expansion modifiers — write git rev:path specs literally
trigger: always_on
---

# RULE: In zsh, `$VAR:path` triggers expansion MODIFIERS (`:s`, `:h`, `:t`…)

## Rule

1. **Never write a bare `$VAR:something` in zsh.** For a git `rev:path` spec, write the literal: `git show "origin/main:src/server/main.cpp"`. Modifiers also apply inside quotes; if a variable is unavoidable, put the `:` outside the expansion: `git show "$rev":"$file"`.
2. **A `git show` that prints a commit header** when you expected file content is the symptom of this bug (the path was swallowed). Never reason on that output.
3. Any shell idiom coming from a bash reflex is checked for zsh before interpreting its result.

## Example

- ❌ **Before (wrong)**: `base=$(git merge-base HEAD origin/main); git show $base:CMakeLists.txt` → zsh applies a modifier → inconsistent output.
- ✅ **After (right)**: `git show "$(git merge-base HEAD origin/main)":CMakeLists.txt` → file content.
