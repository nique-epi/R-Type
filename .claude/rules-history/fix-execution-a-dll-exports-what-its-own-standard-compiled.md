# RULE: A DLL exports what its own standard compiled — an inline member seen only by the caller does not link

## Context

The tests of the results database read the file back through their own `SQLite::Database`. vcpkg builds SQLiteCpp as a DLL on `windows-latest` (`x64-windows`) and as a static library on macOS and Linux.

## Mistake

`ResultsDatabaseTest.cpp` opened its reader with `file()`, a `std::filesystem::path`. It compiled and linked on macOS and Linux; on Windows the link of `server_storage_tests` failed with `LNK2019` on `SQLite::Database::Database(std::filesystem::path const &, int, int, std::string const &)`. Production code was not affected, since `openDatabaseFile()` passes text.

## Root cause

`SQLite/Database.h` guards that constructor with `SQLITECPP_HAVE_STD_FILESYSTEM`, defined from the language standard in force. The class is marked for import on Windows, so the project, at C++20, asks the DLL for a member the DLL may never have compiled. The plan had read the guard and the body of the constructor, not what a DLL build does with it.

## Rule

1. **Open a `SQLite::Database` from a `std::string`.**
2. **When a class of a vcpkg library is marked for import, read the conditions around a member before using it.**
3. **The local build cannot show this.**

## Example

- ❌ **Before (wrong)**: `const SQLite::Database reader{file()};`.
- ✅ **After (right)**: `const SQLite::Database reader{file().string()};`.
