---
description: "A class the tests build holds no SFML object that opens the OpenGL context: SFML aborts on the Linux CI, which has no display"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: A class the tests build never makes an SFML OpenGL resource — the Linux CI has no display and SFML aborts

Why: `LostScreenParts` held an `sf::Texture`; every test of `LostScreen` built an empty one, and on the Linux CI each test process stopped on "Failed to open X11 display", while macOS and Windows passed.

## Rule

1. **Before a class or a struct the tests build holds an SFML object, check whether it derives from `GlResource`** in the headers vcpkg installed: `grep -rn 'GlResource' build/vcpkg_installed/*/include/SFML`. In SFML 3.0.2, `Texture`, `Shader`, `VertexBuffer`, `Context` and `Window` do; `RenderTexture` holds a `Texture`.
2. **Building one, even empty, opens the shared OpenGL context**: the constructor of `GlResource` calls `GlContext::getSharedContext()` (`src/SFML/Window/GlResource.cpp:37`). On Linux without a display, SFML prints "Failed to open X11 display" and calls `std::abort()` (`src/SFML/Window/Unix/Display.cpp:71-72`): the whole test process dies, every test of the executable that reached it with it.
3. **No local run shows it**: macOS and Windows open the context without a window. The Linux job of the CI is the only machine without a display; a green local suite is no verdict.
4. **Hold such an object where the tests can leave it out**: a `std::optional` the tests give empty (`std::optional<sf::Texture> picture`, `.picture = std::nullopt`), or behind an interface the tests replace with a double. The class does without it (draws nothing in its place) and says so in its doc comment.
5. **After the change, check that no test builds one**: `git grep -n -P 'sf::(Texture|Shader|VertexBuffer|RenderTexture|Context)\b' -- tests` prints nothing (`-P`, since `git grep -E` ignores `\b`; it matched `LostScreenFixture.cpp:29` before the fix), and no class a test builds holds one by value.

## Example

- ❌ **Before (wrong)**: `LostScreenParts{.picture = sf::Texture{}, …}` in `LostScreenFixture` → 13 tests of `LostScreen` "Subprocess aborted" on `ubuntu-latest`, green on macOS and on `windows-latest`.
- ✅ **After (right)**: `std::optional<sf::Texture> picture;` in `LostScreenParts` and `LostScreen`, `.picture = std::nullopt` in the fixture; the screen squeezes the picture only when it has one.
