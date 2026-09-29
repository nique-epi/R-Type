---
description: Never reason or decide about the content of a file that was not actually read
trigger: always_on
---

# RULE: Never reason about the content of a file that was not actually read

## Rule

1. **No claim or decision about a file without a read that actually returned its content.** A "File does not exist", empty or error result = STOP: locate the file again (`find`, `git ls-files`) before going on. Never fill in missing content by deduction.
2. **Check the real stack in the project files** before applying general knowledge: SFML version (3, not 2), Asio (standalone, not Boost.Asio), C++ standard (20), `CMakeLists.txt` options. Follow the real code; only borrow from general knowledge what is verified compatible.
3. **Never batch a fallible call** (glob with no match, command with a non-zero exit code) **with writes** (`Write`, `Edit`, `rm`, `git commit`) in the same block. Separate reconnaissance from writing.

## Example

- ❌ **Before (wrong)**: reading `EntityRegistry.hpp` fails → I go on assuming a `getComponent<T>(entity)` method and code against it.
- ✅ **After (right)**: the read fails → `git ls-files | grep -i registry` → read the real file → I see `get<T>(entity)` returning a `std::optional` → I code against reality.
