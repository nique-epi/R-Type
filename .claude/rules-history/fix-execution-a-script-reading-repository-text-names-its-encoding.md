# RULE: A script that reads repository text names its encoding — the shell commands run with no locale

## Context

On 26 October 2026, before pushing a pull request of rules, the frontmatter of its three new rules was to be checked as YAML, as the testing of earlier rules pull requests did, with a Ruby one-liner.

## Mistake

The one-liner read each file with `File.read(ARGV[0])`. The shell the commands run in sets no `LANG` nor `LC_*`, so the system Ruby (2.6.10) took US-ASCII as its default encoding, and `split` stopped on `invalid byte sequence in US-ASCII` for each of the three files. Nothing was concluded from the failed run: it was rerun with `encoding: "UTF-8"`, on the three rules and on a rule whose description holds a `—`, and all four parsed.

## Root cause

The encoding was left to Ruby's default, which follows the locale; the locale of the shell was never read. Python 3.15 in the same shell runs in UTF-8 mode, which hid the difference in earlier scripts.

## Rule

1. Every Ruby script that reads a file of the repository names its encoding (`File.read(path, encoding: "UTF-8")`), or the command starts with `LC_ALL=en_US.UTF-8`.
2. A decode error means the script produced nothing: rerun it before reading any result.
3. Calibrate a script that reads text on a file known to hold non-ASCII characters.

## Example

- ❌ **Before (wrong)**: `File.read(ARGV[0])` in a Ruby check of rule frontmatter → `invalid byte sequence in US-ASCII`.
- ✅ **After (right)**: `File.read(ARGV[0], encoding: "UTF-8")` → the three new rules and the control rule parse.
