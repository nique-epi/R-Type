---
description: A Ruby script that reads a file of the repository names UTF-8, since the shell commands run with no locale and Ruby then reads every file as US-ASCII
trigger: always_on
---

# RULE: A script that reads repository text names its encoding — the shell commands run with no locale

Why: `LANG` and `LC_*` are unset in the shell the commands run in, so the system Ruby (2.6) reads every file as US-ASCII and stops on the first `—` of a rule.

## Rule

1. **Every Ruby script that reads a file of the repository names its encoding**: `File.read(path, encoding: "UTF-8")`, or the command starts with `LC_ALL=en_US.UTF-8`. The rules, the docs and the sources hold non-ASCII characters (`—`, `→`, `❌`). Python 3 needs nothing: it runs in UTF-8 mode here (`sys.flags.utf8_mode` is 1).
2. **`invalid byte sequence in US-ASCII`, or any decode error, means the script produced nothing**: rerun it with the encoding named before reading any result.
3. **Calibrate a script that reads text on a file known to hold non-ASCII characters**, besides the files it is meant for.

## Example

- ❌ **Before (wrong)**: `ruby -ryaml -e 'text = File.read(ARGV[0]); …'` to check the frontmatter of three new rules → `invalid byte sequence in US-ASCII` on each.
- ✅ **After (right)**: `File.read(ARGV[0], encoding: "UTF-8")`, run on the three rules and on `fix-execution-zsh-pipestatus.md`, whose description holds a `—`: all four parse.
