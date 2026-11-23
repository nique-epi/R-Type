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

## Update (2026-10-31) — A function never hands back its const reference parameter

### Context

Pull request #166 (the network simulator) was asked to refuse a negative lag or jitter in `DatagramsInFlight`, since a negative jitter gives `std::uniform_int_distribution` a lower bound above its upper one. The check was written as a function returning the validated value, as `fix-architecture-a-function-does-one-kind-of-step.md` asks, and called in the constructor's initializer list.

### Mistake

`const NetworkConditions& checkedConditions(const NetworkConditions& conditions)` returned `conditions`. The local clang-tidy run before the push reported `bugprone-return-const-ref-from-parameter` as an error: a caller passing a temporary would keep a reference to it after the full expression. The only caller passed a named parameter, so nothing dangled, but the gate failed.

### Root cause

The returned reference was chosen to avoid a copy, without asking what it refers to. The rule covered the caller side (a pointer taken into a temporary), not the function that hands out the reference.

### Rule

1. A function whose parameter is `const T&` never returns that parameter as a reference.
2. A check that hands back the value it validated returns a copy; when the copy is too costly, it returns nothing and the caller goes on with its own object.

### Example

- ❌ `const NetworkConditions& checkedConditions(const NetworkConditions& conditions)` returning `conditions`.
- ✅ `NetworkConditions checkedConditions(const NetworkConditions& conditions)`.

## Update (2026-11-23) — A view member of an aggregate

### Context

A simplification of the master's leaderboard tests moved the GET call of the leaderboard into a shared `leaderboardCall(query)` returning a `RouteCall`.

### Mistake

`RouteCall::path` is a `std::string_view`; the helper filled it with `std::string{LEADERBOARD_URL} + std::string{query}`, a temporary destroyed at the end of the return statement. Every leaderboard read sent a garbage path, and 18 tests failed with `key 'scores' not found`. Caught by the test run before any commit.

### Root cause

The member was filled without reading its type; the original code kept the path in a named `std::string` next to the call, which the helper dropped.

### Rule added

The subsection "A view member of an aggregate is filled from a named variable".
