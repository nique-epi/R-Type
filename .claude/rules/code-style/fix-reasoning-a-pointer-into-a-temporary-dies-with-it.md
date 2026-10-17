---
description: A pointer or reference into an object returned by value lives only until the end of the expression; keep the object in a variable first
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: A pointer into a temporary dies with it — keep the object in a variable before taking a pointer into it

## Rule

1. **A function that returns an object by value hands out a temporary**: a pointer or reference taken into it (`find…`, `writingOf(…)`, `.data()`, `&front()`) dangles once the full expression ends. Store the object in a named `const` variable, then take the pointer from that variable.
2. **Read every call nested inside a lookup** before writing it: in `lookup(producer(…), key)`, if `producer` returns by value and `lookup` returns a pointer or a reference, the line is wrong.
3. **A test that reads zeros, an empty value or a default color where a real one is expected** is the symptom: look for a temporary before suspecting the code under test.

## Example

- ❌ **Before (wrong)**: `const RecordedWriting* label = writingOf(paintingOf(menu), "BACK");` → the record dies at the `;`, the anchor reads (0, 0) and the test fails for no reason in the menu.
- ✅ **After (right)**: `const InterfaceCanvasRecord painting = paintingOf(menu);` then `const RecordedWriting* label = writingOf(painting, "BACK");`.
