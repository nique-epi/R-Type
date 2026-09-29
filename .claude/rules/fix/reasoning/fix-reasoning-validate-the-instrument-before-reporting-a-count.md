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

## Example

- ❌ **Before (wrong)**: `grep -rn "void on[A-Z]" src | wc -l` → "12 handlers"; handlers declared over two lines and lambdas were never seen.
- ✅ **After (right)**: check that two known handlers (one multi-line) are in the output, widen the pattern, give the full list.
