# RULE: In a browser script, a top-level name can be a property of `window` — never name a global `top`, `closed`, `name` or `status`

## Context

Design system of the options menu (RTY-106), built as HTML previews run in a frame, and checked with a WebKit snapshot tool that evaluates a script in each page to press keys and click.

## Mistake

Twice in the same session. A preview declared `var top = Object.assign({}, size, …)` at the top level of its classic script: `window.top` is read-only, the assignment was dropped, and the row of that card showed `undefined`. Later, a check script declared `var closed = !stage.querySelector('.rt-list')`: `window.closed` is a read-only boolean, so the script reported `closed=false` and the dropdown looked like it stayed open after a pick. Three more runs were spent tracing a defect that did not exist before the name was recognized.

## Root cause

A top-level `var` in a classic script is a property of `window`; the names of existing `window` properties are taken, some of them read-only, and assigning them fails silently outside strict mode.

## Rule

1. Never declare a top-level `var` or function named after a `window` property: `top`, `parent`, `self`, `closed`, `name`, `status`, `length`, `origin`, `history`, `location`, `open`, `close`, `event`, `screen`, `frames`. Use a precise name, or wrap the script in a function.
2. A value that reads `undefined`, `false` or `[object Window]` right after being assigned is the symptom: rename it and run again before believing the result.

## Example

- ❌ **Before (wrong)**: `var top = …` → the row shows `undefined`; `var closed = …` → `closed=false`, read as a defect.
- ✅ **After (right)**: `var largest = …`, `var listGone = …` → the values are the ones assigned.
