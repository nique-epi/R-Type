# RULE: In zsh, `$VAR:path` triggers expansion MODIFIERS (`:s`, `:h`, `:t`…)

This file holds the updates made to the rule after it was written; the first incident is told in the rule's example.

## Update (2026-10-17) — a push refspec built from a variable

### Context

A rule commit for #79 was built on a temporary index, so that the user's working tree, on `feat/options-menu`, did not change while the user was testing the client. The commit id was kept in `$COMMIT`, and pushed with `git push origin "$COMMIT:refs/heads/chore/rules-after-options-menu"`.

### Mistake

zsh read `:r` as the modifier that removes an extension: the refspec became `<sha>efs/heads/chore/rules-after-options-menu`, and the push failed with `src refspec … does not match any`. A first call that printed only its last line showed `error: failed to push some refs`, and the remote SHA, read right after, was still the old one; the full message showed the mangled refspec.

### Root cause

The rule named `git show $rev:path`; a refspec was not recognized as the same `$name:word` form, and the quotes, which do not stop modifiers, gave a false sense of safety.

### Rule (added)

1. **A push refspec is a `rev:path` too**: `git push origin "$commit:refs/heads/<branch>"` reads `:r` as the modifier that drops an extension, and pushes `<sha>efs/heads/<branch>`. Write `"${commit}:refs/heads/<branch>"`: the braces end the name before the colon. The same goes for `host:port` and `user@host:path` built from a variable.
2. **`src refspec <sha>efs/…` (or any word missing its first letters after a colon) is the symptom**: the push did not happen; fix the braces and push again.
3. **Before sending a command, read every `$` directly followed by a name and a colon**, quoted or not, and add the braces.

## Update (2026-10-25) — a branch kept in a variable for a read

To explain the music to the user, `b=feat/client-menu-click; git show $b:src/client/Audio/MusicPlayer/MusicPlayer.cpp` failed with `ambiguous argument 'feat/client-menu-click.cpp'`: zsh read `:s` as a modifier, although point 3 of the rule asks to read every `$name:` before sending. Added: a revision used in a read is written out in the command, never kept in a short variable.
