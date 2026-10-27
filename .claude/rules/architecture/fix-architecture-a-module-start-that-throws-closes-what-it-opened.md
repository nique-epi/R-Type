---
description: "What a module's start() opens, its stop() closes; a start() that throws closes what it already opened before the exception leaves it, since Application never calls stop() on that module"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: What a module's `start()` opens, its `stop()` closes — and a `start()` that throws closes it itself

Why: when a module's `start()` throws, `Application` stops the modules started before it and never calls `stop()` on the one that threw; a module that opens two things and fails on the second keeps the first one open.

## Rule

1. **`stop()` closes everything `start()` opened, in the reverse order.** Each resource opened in `start()` is kept in a member (the socket, the thread, the `SubscriptionHandle`) and released in `stop()`. A subscription is released with `EventBus::unsubscribe(handle)`: the handle is only a number, and destroying it unsubscribes nothing.
2. **A `start()` that opens more than one thing releases what it already opened before the exception leaves it.** Either open each resource into a local object whose destructor releases it, and move the locals into the members only once every step succeeded; or catch, release in the reverse order, and rethrow. Never count on `stop()` for it: `Application::startModules()` does not call it on the module whose `start()` threw.
3. **Test the failure, not only the success**: a module whose second step fails, then a check that the first resource is released (the thread joined, the port bound again by the test, the subscription gone: a second `unsubscribe` of its handle returns `false`).

## Example

- ❌ **Before (wrong)**: `start()` launches the network thread into a `std::thread` member, then binds the socket, which throws because the port is taken → `Application` throws `ModuleStartFailedException` without calling this module's `stop()`; when the module is destroyed, its thread is still joinable and `std::thread`'s destructor calls `std::terminate`, so the error message is never shown.
- ✅ **After (right)**: `start()` binds the socket first, then launches the thread; when the thread cannot start, the socket held in a local is closed by its destructor before the exception leaves `start()`; both move into the members only once both succeeded.
