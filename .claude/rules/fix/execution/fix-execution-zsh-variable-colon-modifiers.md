---
description: In zsh, $VAR:something triggers expansion modifiers — write git rev:path specs literally
trigger: always_on
---

# RULE: In zsh, `$VAR:path` triggers expansion MODIFIERS (`:s`, `:h`, `:t`…)

## Rule

1. **Never write a bare `$VAR:something` in zsh.** For a git `rev:path` spec, write the literal: `git show "origin/main:src/server/main.cpp"`. Modifiers also apply inside quotes; if a variable is unavoidable, put the `:` outside the expansion: `git show "$rev":"$file"`.
2. **A `git show` that prints a commit header** when you expected file content is the symptom of this bug (the path was swallowed). Never reason on that output.
3. Any shell idiom coming from a bash reflex is checked for zsh before interpreting its result.

### Every `$name:` is written `${name}:`, refspecs included

1. **A push refspec is a `rev:path` too**: `git push origin "$commit:refs/heads/<branch>"` reads `:r` as the modifier that drops an extension, and pushes `<sha>efs/heads/<branch>`. Write `"${commit}:refs/heads/<branch>"`: the braces end the name before the colon. The same goes for `host:port` and `user@host:path` built from a variable.
2. **`src refspec <sha>efs/…` (or any word missing its first letters after a colon) is the symptom**: the push did not happen; fix the braces and push again.
3. **Before sending a command, read every `$` directly followed by a name and a colon**, quoted or not, and add the braces.

### A revision used in a read is written out, not kept in a variable

1. **To read files of a branch or a commit, write its name in the command**: `git show feat/client-menu-click:src/client/Audio/MusicPlayer/MusicPlayer.cpp`, never a short variable such as `b=feat/…; git show $b:src/…`, which zsh reads as the modifier `:s` and turns into `<branch>.cpp`.
2. **`ambiguous argument '<name>.cpp'`, or a path that lost its first letters, is the symptom**: nothing was read; rerun with the name written out.

## Example

- ❌ **Before (wrong)**: `base=$(git merge-base HEAD origin/main); git show $base:CMakeLists.txt` → zsh applies a modifier → inconsistent output.
- ✅ **After (right)**: `git show "$(git merge-base HEAD origin/main)":CMakeLists.txt` → file content.
