---
description: Never reason or decide about the content of a file that was not actually read
trigger: always_on
---

# RULE: Never reason about the content of a file that was not actually read

## Rule

1. **No claim or decision about a file without a read that actually returned its content.** A "File does not exist", empty or error result = STOP: locate the file again (`find`, `git ls-files`) before going on. Never fill in missing content by deduction.
2. **Check the real stack in the project files** before applying general knowledge: SFML version (3, not 2), Asio (standalone, not Boost.Asio), C++ standard (20), `CMakeLists.txt` options. Follow the real code; only borrow from general knowledge what is verified compatible.
3. **Never batch a fallible call** (glob with no match, command with a non-zero exit code) **with writes** (`Write`, `Edit`, `rm`, `git commit`) in the same block. Separate reconnaissance from writing.

### A design document describes the rendering it read, not the one it expects

1. **Before writing how the client draws** (scaling, blending, fonts, timings) in a page, a design system, a mockup or a plan, open the function that does it (`integerScaleFactor`, `PixelCanvas::show`, `InterfaceCanvas`) and write its behavior with its name.
2. **A round example is checked against every row of the table it comes from**: "×2 at 960 × 540, ×4 at 1920 × 1080" says nothing of 1280, 1600 and 2560 in `WINDOW_SIZES`, which are not multiples of 480.

### A clang-tidy check is cited only after reading `.clang-tidy`

1. **Before naming a check as what enforces or forbids something** (in a rule, a plan, a doc comment or a pull request), read its line in `.clang-tidy`: the `Checks` list starts from `*` and turns checks off by name, so a check that exists in clang-tidy may be off in this repository.
2. **When the check is off, say so next to the guideline it stands for**, and cite the guideline instead: "Core Guidelines C.12; `.clang-tidy` turns its check off".

### A file the user says they have is found before it is described

1. **When the user names an asset they already have** (a music, an image, a font), look for the file on disk (`find ~/Downloads ~/Desktop ~/Music . -maxdepth 4 -iname '*<word>*'`) before writing anything about it in a brief, a plan or a design.
2. **Read the names from the file, not from the message**: two names on two lines may be one item's title and its author, as a music player shows them, and a prefix in the file name may say where it comes from.

## Example

- ❌ **Before (wrong)**: reading `EntityRegistry.hpp` fails → I go on assuming a `getComponent<T>(entity)` method and code against it.
- ✅ **After (right)**: the read fails → `git ls-files | grep -i registry` → read the real file → I see `get<T>(entity)` returning a `std::optional` → I code against reality.
- ❌ **Before (wrong)**: the parameter rule said a struct holds no reference member "(`cppcoreguidelines-avoid-const-or-ref-data-members`)", from a note of an earlier session; `.clang-tidy` turns that check off.
- ✅ **After (right)**: read `.clang-tidy` → the check is off → the rule cites Core Guidelines C.12 and says nothing checks it.
- ❌ **Before (wrong)**: the user wrote "Spacesuit Suite" and "Stationary Sign" on two lines as the menu music → a research brief asked for "the two tracks".
- ✅ **After (right)**: `find ~/Downloads -iname '*spacesuit*'` → `ES_Spacesuit Suite - Stationary Sign.mp3`: one track, Spacesuit Suite, by Stationary Sign, downloaded from Epidemic Sound (`ES_`).
