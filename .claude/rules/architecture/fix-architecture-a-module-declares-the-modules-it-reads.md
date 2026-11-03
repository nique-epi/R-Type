---
description: "A module that reads a service of another module names that module in dependencies() and requires the service; find() with a default is only for a service it works without"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: A module names in `dependencies()` every module whose service it reads

Why: the storage module read the port of the network module with `find()` and fell back on the default port, without naming `network` in `dependencies()`; a list without the network module named the file after the wrong port, with no error.

## Rule

1. **Before calling `context.find<I>()` or `context.require<I>()` in `install()`, name the module that provides `I`** and check that it is in `dependencies()`. The application then refuses a list where it is missing or listed after (`MissingDependencyException`), and `module <name> off` refuses to turn it off while the reader stays on.
2. **A service the module cannot do without is read with `require()`, never with `find()` and a default value.** A default turns a wrong list into a wrong result nobody sees.
3. **`find()` is for a service the module works without**, and its doc comment says what the module does then.
4. **The pages that list what each module needs** (`server.md`, `server.cfg.example`) gain the dependency in the same change.

## Example

- ❌ **Before (wrong)**: `listeningPort_ = context.find<IListeningPort>();` then `listeningPort_ != nullptr ? listeningPort_->listeningPort() : network::DEFAULT_SERVER_PORT`, with `dependencies()` returning the logger only.
- ✅ **After (right)**: `dependencies()` returns the logger and `network`, `listeningPort_ = &context.require<IListeningPort>();`, and a test builds the application without the network module and expects `MissingDependencyException`.
