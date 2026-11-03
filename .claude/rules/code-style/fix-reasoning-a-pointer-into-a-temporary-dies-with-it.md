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

### A function never hands back its const reference parameter

1. **A function whose parameter is `const T&` never returns that parameter as a reference**: a caller that passes a temporary keeps a reference to it after the full expression (`bugprone-return-const-ref-from-parameter`, an error of the tidy gate).
2. **A check that hands back the value it validated returns a copy**: `NetworkConditions checkedConditions(const NetworkConditions&)`. When the copy is too costly, the check returns nothing and the caller goes on with its own object.

## Example

- ❌ **Before (wrong)**: `const RecordedWriting* label = writingOf(paintingOf(menu), "BACK");` → the record dies at the `;`, the anchor reads (0, 0) and the test fails for no reason in the menu.
- ✅ **After (right)**: `const InterfaceCanvasRecord painting = paintingOf(menu);` then `const RecordedWriting* label = writingOf(painting, "BACK");`.
- ❌ **Before (wrong)**: `const NetworkConditions& checkedConditions(const NetworkConditions& conditions)` returning `conditions` → `returning a constant reference parameter may cause use-after-free`.
- ✅ **After (right)**: `NetworkConditions checkedConditions(const NetworkConditions& conditions)`: a copy of five fields, once per construction of `DatagramsInFlight`.
