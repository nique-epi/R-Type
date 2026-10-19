# RULE: A function takes at most three parameters — values that go together become one named object

## Context

Pull request of the client waiting room (RTY-107). A teammate reviewing it commented on the constructor of `LobbyScreen`: too many parameters, to be added to the rules, and composition and inheritance to watch.

## Mistake

The constructor took seven parameters:
- the navigator and the connection;
- the canvas and the playfield mapping;
- the key bindings and a key naming function, only to read one key and its name;
- the motion preference.

The same branch had grown `IInterfaceCanvas::drawImage` from four to five parameters (`scale`), and the painting of a bay to five public and six private parameters.

Measured on every `.cpp` the branch changed, with `readability-function-size` and `ParameterThreshold: 3`, 15 functions were over the limit:
- 9 written or changed by the branch: the `LobbyScreen` constructor, `InterfaceCanvas::drawImage` and its recording double, and six functions of `HangarBay.cpp`;
- 6 that were already there: the `Application`, `ConnectionScreen`, `OptionsScreen` and `Button` constructors, `SfmlDrawSurface::draw` and `MenuBuilder::tabs`.

## Root cause

No rule and no check set a limit. `.clang-tidy` does not enable a parameter threshold, so each new parameter was added where it was needed, one at a time. The constructor took what the screen needed to compute (bindings, naming) instead of what it keeps (a key and its name).

## Sources read

- C++ Core Guidelines, I.23 "Keep the number of function arguments low", read in `CppCoreGuidelines.md` of the isocpp/CppCoreGuidelines repository:
  - reason: confusion and cost;
  - the two usual causes: a missing abstraction, and a function doing more than one job;
  - the note "Try to use fewer than four (4) parameters";
  - the alternatives: group arguments into meaningful objects, or use default arguments and overloads;
  - its enforcement says the guideline cannot be checked directly.
- Martin Fowler, refactoring catalog, *Introduce Parameter Object* and *Preserve Whole Object*: https://refactoring.com/catalog/introduceParameterObject.html, https://refactoring.com/catalog/preserveWholeObject.html.
- clang-tidy `readability-function-size` (`ParameterThreshold`): https://clang.llvm.org/extra/clang-tidy/checks/readability/function-size.html. The measurement was calibrated on the seven-parameter constructor, which it reported.

## Rule

1. **Every function, constructor or lambda written or changed takes at most three parameters.** This is the limit of the C++ Core Guidelines, I.23 "Keep the number of function arguments low": "Try to use fewer than four (4) parameters". The implicit `this` does not count. Every other parameter counts, a canvas or a navigator included.
2. **Before writing a fourth parameter, look at what the values are, and take the first option that fits:**
   1. **Pass the whole object the values come from** (Fowler, *Preserve Whole Object*): `paintPlayerNumber(canvas, bay)`, not `paintPlayerNumber(canvas, bay.topLeft, bay.playerNumber, open)`.
   2. **Group values that only make sense together into a struct named after what they are** (Fowler, *Introduce Parameter Object*; I.23: "group arguments into meaningful objects"). Examples: `TextureArea{assetId, rectangle}`, `BayMotion{elapsed, preference}`.
      - The dependencies a class is built from go in a struct nested in that class: `KeySlot::Wiring`, `LobbyScreen::Parts`. The constructor takes that struct, plus any object it takes ownership of (point 4).
   3. **Give a member a default when most callers pass the same value** (I.23: "use default arguments or overloads"), rather than a parameter every caller repeats: `ImagePlacement::scale = NATIVE_SCALE`.
   4. **Compute the value before the call when the callee only needs the result**: the bay passes a ship frame, not the four values it comes from.
3. **At the call site, build such a struct with designated initializers and name its type**, for example `TextureArea{.assetId = …, .rectangle = …}`. The reader sees which value is which, and two values of the same type cannot be swapped silently.
4. **What a struct may hold:**
   - **No reference member in a struct that is copied or moved** (Core Guidelines C.12, "Don't make data members `const` or references in a copyable or movable type"; `.clang-tidy` turns its check off, so nothing catches it). A dependency that must exist and that the struct does not own is a `std::reference_wrapper`, never a pointer that could be null.
   - **An object the callee takes ownership of stays its own `std::unique_ptr` parameter**, out of the struct (Core Guidelines R.32, "Take a `unique_ptr<widget>` parameter to express that a function assumes ownership of a `widget`"). The signature then shows the transfer. Inside an aggregate handed to `std::make_unique`, the static analyzer reported a potential leak of it (`clang-analyzer-cplusplus.NewDeleteLeaks` on `LobbyScreen::Parts`).
5. **Never group unrelated values just to get under the count.** A struct called `…Arguments` or `…Params` that holds whatever the function needs is the same long list with a name on it. When the values share no meaning, the function does too much (I.23, "one function, one responsibility"): split it.
6. **Measure every `.cpp` the branch changes before the push.** Use the SDK flags of `fix-execution-run-clang-tidy-before-push.md`:
   ```bash
   clang-tidy -p build --quiet --config='{Checks: "-*,readability-function-size", CheckOptions: {readability-function-size.ParameterThreshold: 3}}' <changed .cpp files>
   ```
   - The check reports each definition with more than three parameters, with a note "N parameters (threshold 3)". A pure virtual method has no body, so the check reports it at its implementations.
   - Before trusting an empty result, calibrate the command on a function known to be over the limit.
7. **A function over the limit that the change does not touch is reported, not extended.** Adding a parameter to it, or copying its list into a new function, extends it (`fix-process-rework-preexisting-inconsistencies.md`).

## Example

- ❌ **Before (wrong)**:
  - `LobbyScreen(IScreenNavigator&, IServerConnection&, std::unique_ptr<IInterfaceCanvas>, PlayfieldMapping, const KeyBindings&, const KeyNaming&, MotionPreference)`;
  - `drawImage(assetId, source, topLeft, scale, visibleArea)`;
  - `paintPilot(canvas, topLeft, color, pilot, elapsed, motion)`.
- ✅ **After (right)**:
  - `LobbyScreen(Parts parts, std::unique_ptr<IInterfaceCanvas> canvas)`, built as `LobbyScreen(LobbyScreen::Parts{.navigator = screens_, …, .motion = motionPreference()}, interfaceCanvas())`;
  - `drawImage(TextureArea image, ImagePlacement placement, sf::IntRect visibleArea)`;
  - `paintPilot(canvas, bay, pilot)`, with the ship painted next to it from `shipFrameOf(color, readiness, motion)`.
