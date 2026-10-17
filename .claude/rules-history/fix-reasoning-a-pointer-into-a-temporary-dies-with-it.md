# RULE: A pointer into a temporary dies with it — keep the object in a variable before taking a pointer into it

## Context

Options menu of the client (RTY-106), first run of the new tests: tabbed panel tests in `MenuTest.cpp` and `MenuBuilderTest.cpp`, and the window size chart test.

## Mistake

Six new tests failed: the anchors of writings read (0, 0) and a legend color read all zeros. Each one wrote `writingOf(paintingOf(…), "…")` or `writingOf(chartOf(…), "…")`: `paintingOf` and `chartOf` return an `InterfaceCanvasRecord` by value, `writingOf` returns a pointer into its vector, and the record was destroyed at the end of the statement, so the assertions read freed memory. The plan had shown the same lines, so they had been approved without anyone noticing.

## Root cause

The existing tests store the painting in a variable first; the new ones folded the two calls into one line for brevity without checking what `writingOf` returns.

## Rule

1. A function that returns an object by value hands out a temporary: a pointer or reference taken into it dangles once the full expression ends. Store the object in a named `const` variable, then take the pointer from that variable.
2. Read every call nested inside a lookup before writing it.
3. A test that reads zeros, an empty value or a default color where a real one is expected is the symptom.

## Example

- ❌ `const RecordedWriting* label = writingOf(paintingOf(menu), "BACK");`.
- ✅ `const InterfaceCanvasRecord painting = paintingOf(menu);` then `writingOf(painting, "BACK")`.
