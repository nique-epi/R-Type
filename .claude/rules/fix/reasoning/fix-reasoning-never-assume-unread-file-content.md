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

## Example

- ❌ **Before (wrong)**: reading `EntityRegistry.hpp` fails → I go on assuming a `getComponent<T>(entity)` method and code against it.
- ✅ **After (right)**: the read fails → `git ls-files | grep -i registry` → read the real file → I see `get<T>(entity)` returning a `std::optional` → I code against reality.
