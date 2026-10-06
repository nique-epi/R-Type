# History: platform code in one file per platform

## Context
The plan of the client resource manager had to find the assets folder whatever the launch folder, which needs the path of the running executable: `GetModuleFileNameW` on Windows, `_NSGetExecutablePath` on macOS, `/proc/self/exe` on Linux.

## Mistake
The plan put the three systems in one function, `reportedExecutablePath()`, as `#ifdef` branches inside its body, each branch throwing `UnknownExecutablePathException` on its own error code. The user found it dirty and asked for the error-handling practices of C++ and of game engines, and for clean code that follows the conventions.

## Root cause
The shortest edit was chosen: one function, one file, and a throw where each system call can fail. Nothing described how platform code is split, nor which layer turns a system failure into an error.

## Rule
See `.claude/rules/architecture/fix-architecture-platform-code-one-file-per-platform.md`. Engines split platform code by file: SDL has one `SDL_sysfilesystem.c` per system under `src/filesystem/` (`unix/`, `windows/`, `cocoa/`...), and `SDL_GetBasePath` returns NULL on failure instead of stopping the program. The C++ Core Guidelines E.2 and E.18 ask to throw when a function cannot perform its task and to keep explicit `try`/`catch` to a minimum, which places the throw in the caller that knows the failure is fatal.

## Example
- ❌ **Before (wrong)**: one function with `#ifdef _WIN32` / `#elif defined(__APPLE__)` / `#elif defined(__linux__)` branches, each throwing.
- ✅ **After (right)**: `std::optional<std::filesystem::path> executablePath();`, one `.cpp` per system returning `std::nullopt` on failure, one `throw` where the assets folder is located at launch.
