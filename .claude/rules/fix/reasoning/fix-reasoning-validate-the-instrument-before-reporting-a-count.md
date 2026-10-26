---
description: A count, total or list produced by a homemade filter is validated on known cases before being reported or driving a deletion
trigger: always_on
---

# RULE: A count is only worth something if the instrument that produced it was validated on a known case

## Rule

Before reporting a **count, total or list** produced by a homemade filter (`grep`, regex, `awk`, `jq`, script):

1. **Validate the instrument on a known case**: pick two or three items that *must* appear — preferably the most atypical ones (mixed case, spaces, templates, multi-line declarations) — and check they are in the output. If they are missing, the instrument is wrong.
2. **Cross-check with an independent total** (`wc -l`, file count, raw count). An unexplained gap is the signal.
3. **Give the list, not just the number.** A list can be inspected at a glance; a number cannot.
4. **Distrust restrictive character classes** (`[A-Z_]`, `\w`) on real identifiers.

### An unvalidated filter never drives a deletion

1. Counting wrong costs a wrong number; deleting wrong costs the work. Before any destructive action derived from a filter, validate the instrument.
2. **Regex-based static analysis does not see every use** (macros, templates, calls through function pointers, registration by name): a search for "unused" is a lower bound of usage. The reliable measurement is the **build** and the **tests** after deletion.
3. Commit before a mass deletion.

### A parser of test results is validated on every shape of test name

1. **Before trusting a script that reads which tests failed**, feed it a known failure of each name shape the suite holds: a plain `Suite.Test`, a fixture, and a value-parameterized test, whose CTest name carries the printed parameter (spaces, quotes, parentheses). A pattern such as `(\S+)` silently skips the last kind.
2. **When the script reports "no test red", read the raw log before believing it**: a variant that turns nothing red is either a gap in the tests or a gap in the parser, and only the log tells which.

### A check written into a rule is calibrated with the command the rule prints

1. **Calibrate the exact command a rule will tell the next reader to run**, tool and flags included, on its known positive case: `git grep -P '<pattern>' <commit> -- <path>`, never an equivalent pipe such as `git show <commit>:<path> | grep -E '<pattern>'`. Two tools that look alike read patterns differently: `git grep -E` (git 2.53) ignores `\b`, where `grep -E` honours it.
2. **Write the calibration in the rule in the same form**, so the next reader reruns the very command that was measured.

### A filter of diff lines keeps every kind of line it must show

1. **To list the added and removed lines of a diff, drop only its two file headers**: `grep -E '^[-+]' | grep -v -E '^(\+\+\+|---) '`. A pattern such as `^[-+][^-+]` also drops every added or removed line that starts with `-` or `+`: every Markdown bullet (`+- …`) and every removed list item.
2. **Before reading a staged diff through a filter, check it shows a line of each kind the change holds** (a bullet, a table row, a code line). A filter that shows the table row and none of the bullets of an index is the symptom.

## Example

- ❌ **Before (wrong)**: `grep -rn "void on[A-Z]" src | wc -l` → "12 handlers"; handlers declared over two lines and lambdas were never seen.
- ✅ **After (right)**: check that two known handlers (one multi-line) are in the output, widen the pattern, give the full list.
- ❌ **Before (wrong)**: `git diff --cached -U0 -- .claude/CLAUDE.md | grep -E '^[-+][^-+]'` showed only the changed table row of the index; the two index bullets added (`+- …`), one of them meant for the next commit, were not printed.
- ✅ **After (right)**: `grep -E '^[-+]' | grep -v -E '^(\+\+\+|---) '` prints the row and both bullets, and the stray bullet is seen before the commit.
