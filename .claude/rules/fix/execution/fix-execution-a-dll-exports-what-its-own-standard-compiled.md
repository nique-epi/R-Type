---
description: A class imported from a DLL exports what the DLL was compiled with; on Windows the std::filesystem::path constructor of SQLite::Database is not exported, so a database opens from a std::string
trigger: always_on
---

# RULE: A DLL exports what its own standard compiled — an inline member seen only by the caller does not link

Why: vcpkg builds SQLiteCpp as a DLL on Windows, at an older C++ standard than the project; a test that opened a database from a `std::filesystem::path` compiled everywhere and failed to link on the Windows job only.

## Rule

1. **Open a `SQLite::Database` from a `std::string`, in the sources and in the tests**, never from a `std::filesystem::path`: `SQLite::Database reader{file().string()}`. Production code goes through `storage::openDatabaseFile()`, which takes UTF-8 text.
2. **When a class of a vcpkg library is marked for import, a member under a language-standard condition in its header may not exist in the DLL**: the header shows it to a caller at C++20, and the DLL, built at its own standard, never compiled it. Read the conditions around the member before using it.
3. **The local build cannot show this**: macOS and Linux link static libraries here, so the member is simply compiled inline. `LNK2019` on a `__declspec(dllimport)` symbol that the header declares is the symptom.

## Example

- ❌ **Before (wrong)**: `const SQLite::Database reader{file()};`, with `file()` returning a `std::filesystem::path` → `LNK2019: unresolved external symbol __declspec(dllimport) SQLite::Database::Database(std::filesystem::path const &, …)` on `windows-latest`.
- ✅ **After (right)**: `const SQLite::Database reader{file().string()};`.
