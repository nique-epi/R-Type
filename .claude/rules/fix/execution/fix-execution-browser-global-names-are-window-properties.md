---
description: A top-level name in a browser script can be a read-only property of window; never name a global top, closed, name or status
trigger: always_on
---

# RULE: In a browser script, a top-level name can be a property of `window` — never name a global `top`, `closed`, `name` or `status`

## Rule

1. **Never declare a top-level `var` or function named after a `window` property** in a classic script, a preview page or a script handed to a browser to evaluate (`evaluateJavaScript`, `javascript_tool`): `top`, `parent`, `self`, `closed`, `name`, `status`, `length`, `origin`, `history`, `location`, `open`, `close`, `event`, `screen`, `frames`. Use a precise name (`largestSize`, `listGone`) or wrap the script in a function.
2. **A value that reads `undefined`, `false` or `[object Window]` right after being assigned is the symptom**: a read-only property swallowed the assignment. Rename it and run again before believing the result, and never report it as a defect of the code under test.

## Example

- ❌ **Before (wrong)**: `var top = Object.assign({}, size, { label: 'AT THE LARGEST' })` in a preview → the row shows `undefined`; `var closed = !stage.querySelector('.rt-list')` in a check script → `closed=false`, read as "the list stays open".
- ✅ **After (right)**: `var largest = …`, `var listGone = …` → the row shows its label, and the check reports the list gone.
