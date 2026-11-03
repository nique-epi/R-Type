# RULE: A module names in `dependencies()` every module whose service it reads

## Context

The storage module names the SQLite file of a server after the port it listens on, so two servers on one machine never share a leaderboard. The port comes from `IListeningPort`, which the network module provides.

## Mistake

`ServerStorageModule::install()` read the port with `context.find<IListeningPort>()` and `databaseFile()` fell back on `DEFAULT_SERVER_PORT` when nothing provided it. `dependencies()` returned the logger only. A review found that a list of modules without the network module, or with it listed after the storage module, would name the file after port 4242 with no error, whatever port the server listened on.

## Root cause

The default was written so the tests of the storage module could run without a network module, and the dependency it hid was never named.

## Rule

1. Name the module that provides each service read in `install()`, and check it is in `dependencies()`.
2. A service the module cannot do without is read with `require()`, never with `find()` and a default.
3. `find()` is for a service the module works without, and its doc comment says what happens then.
4. The pages that list what each module needs gain the dependency in the same change.

## Example

- ❌ **Before (wrong)**: `find<IListeningPort>()` and a fallback on `DEFAULT_SERVER_PORT`, `dependencies()` returning the logger only.
- ✅ **After (right)**: `network` in `dependencies()`, `require<IListeningPort>()`, the recording network double providing the port as the real module does, and a test that expects `MissingDependencyException` without it.
