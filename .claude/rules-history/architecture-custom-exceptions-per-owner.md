# History: domain errors are raised through the custom exceptions of their owner

The rule was imported from another project when R-Type started: its original incident is not recorded here.

## Update (2026-11-03) — A new top library has its `Exceptions/` folder from its first commit

### Context

The leaderboard of a server added `src/storage`, a library that opens every SQLite file of the project. It raises three errors: a file SQLite refuses, a file that cannot use a write-ahead log, and a file written by a newer program.

### Mistake

`StorageException.hpp` and `StorageException.cpp` sat at the root of `src/storage/`, compiled into `rtype_storage`, while every other library of `src/` that raises an error keeps its root exception in an `Exceptions/` folder with a target of its own (`src/concurrency/Exceptions/`, `src/logging/Exceptions/`). The rule already asked for the folder; the gap was found while rereading the rules after `main` renamed it to name owners instead of modules.

### Root cause

The library held one exception class, so a folder for it looked heavier than the file; the rule was not checked against the new library, and no command lists the libraries without the folder.

### Rule

A library added under `src/` that raises an error puts its root exception in `src/<library>/Exceptions/`, with a target `rtype_<library>_exceptions` linked `PUBLIC` by the library; before the push, `for dir in src/*/; do [ -d "${dir}Exceptions" ] || echo "$dir"; done` prints only `src/identifiers/`.

### Example

- ❌ **Before (wrong)**: `src/storage/StorageException.{hpp,cpp}` in `rtype_storage`; the loop prints `src/identifiers/` and `src/storage/`.
- ✅ **After (right)**: `src/storage/Exceptions/StorageException.{hpp,cpp}` in `rtype_storage_exceptions`, linked `PUBLIC` by `rtype_storage`; the loop prints `src/identifiers/` only.
