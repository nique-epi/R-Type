---
description: "Platform code: one declaration, one source file per platform, failure reported by the return value; never #ifdef inside a function body"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: Platform code lives in one file per platform, behind one declaration that reports failure through its return value

## Rule

1. **One platform-neutral declaration** in a header (`ExecutablePath.hpp`): callers never see which system answers.
2. **One `.cpp` per platform** (`ExecutablePathLinux.cpp`, `ExecutablePathMacOs.cpp`, `ExecutablePathWindows.cpp`), each holding a complete implementation for that system only. **Never an `#ifdef` inside a function body**, never two systems interleaved in one function.
3. **The platform layer does not throw.** It reports failure through its return value (`std::optional`, a status), uses the `std::error_code` overloads of `std::filesystem`, and turns the system's error codes into that value. The caller that knows what the failure means (stop at launch, fall back, retry) decides, and throws only there.
4. **Check the library first**: a dependency already in the project (SFML, Asio, the standard library) may answer; only then write platform code.
5. **Every platform file is compiled and linted somewhere**: name the CI job or the machine that builds each one, and state the platforms nobody builds.

## Example

- ❌ **Before (wrong)**: one `reportedExecutablePath()` holding `#ifdef _WIN32` / `#elif defined(__APPLE__)` / `#elif defined(__linux__)` branches, each throwing `UnknownExecutablePathException` on its own error code.
- ✅ **After (right)**: `std::optional<std::filesystem::path> executablePath();` in `ExecutablePath.hpp`, one `.cpp` per system returning `std::nullopt` on failure, and a single `throw` in the code that locates the assets folder at launch.
