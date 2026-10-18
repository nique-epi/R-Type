# RULE: File name = `PascalCase` name of the main class

This rule was imported when R-Type started; its original incident belongs to another repository. This file holds the updates made here.

## Update (2026-10-17) — One test file per class tested

### Context

Options menu of the client. The new drawing pieces (`Outline`, `CheckBox`, `KeyCap`) and three of the new row controls (`Switch`, `ChoiceSelector`, `LinkText`) each got their tests, written at the same time as the pieces.

### Mistake

The tests were grouped: `tests/client/Widgets/DrawingPiecesTest.cpp` held the `OutlineTest`, `CheckBoxTest` and `KeyCapTest` suites, and `tests/client/Widgets/RowControls/ChoiceControlsTest.cpp` the `SwitchTest`, `ChoiceSelectorTest` and `LinkTextTest` suites, while `Slider`, `Dropdown` and `KeySlot` each had their own file. Neither name exists in `src/`. The fresh placement review reported both.

### Root cause

The pieces were small and drawn alike, and two of them shared a helper (`litPixels`), so one file looked simpler. The rule said "named after what it tests", and an invented group name was read as satisfying it.

### Rule (added)

- A test file holds the tests of **one** class or one free-function header. The suites of several classes are never grouped under a name `src/` does not have, however small or alike the classes are: `OutlineTest.cpp`, `CheckBoxTest.cpp` and `KeyCapTest.cpp`, not `DrawingPiecesTest.cpp`. A helper the files share goes in a test helper file (`litPixelsOf()` in `RecordingInterfaceCanvas.hpp`).
