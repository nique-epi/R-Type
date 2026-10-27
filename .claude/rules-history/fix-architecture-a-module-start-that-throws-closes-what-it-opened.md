# History: what a module's start() opens, its stop() closes, and a start() that throws closes it itself

## Context
Pull request #124 added `IModule` and `Application` to `rtype_engine`. When a module throws from `start()`, `Application::startModules()` stops the modules started before it, the last started first, then throws `ModuleStartFailedException`; the module that threw is not stopped. The doc comment of `Application::start()` says so, but nothing tells the author of a module what that means for them. The user asked for rules that tell `install()` and `start()` apart after an analysis of the pull request raised the point.

## Mistake
No defect was shipped; the rule was asked for before any module was written. The gap: a module whose `start()` opens two things (a socket, then a thread) and fails on the second keeps the first one open, since its `stop()` is never called.

## Root cause
- `stop()` is the only release a module author thinks of, and `Application` deliberately does not call it after a failed `start()`: the module may be half started, and `stop()` would then release what was never opened.
- `SubscriptionHandle` is a plain struct holding a number, so a subscription is released only by `EventBus::unsubscribe(handle)`, never by letting the handle go.
- A joinable `std::thread` destroyed without `join()` calls `std::terminate`, which turns a start failure into a crash that hides the error message.

## Rule

1. **`stop()` closes everything `start()` opened, in the reverse order**, each resource kept in a member; a subscription is released with `EventBus::unsubscribe(handle)`.
2. **A `start()` that opens more than one thing releases what it already opened before the exception leaves it**: locals whose destructors release them, moved into the members once every step succeeded, or catch, release in the reverse order, and rethrow.
3. **Test the failure, not only the success**: a module whose second step fails, then a check that the first resource is released.

## Example

- ❌ **Before (wrong)**: `start()` launches the network thread into a `std::thread` member, then binds the socket, which throws because the port is taken → `Application` throws `ModuleStartFailedException` without calling this module's `stop()`; when the module is destroyed, its thread is still joinable and `std::thread`'s destructor calls `std::terminate`, so the error message is never shown.
- ✅ **After (right)**: `start()` binds the socket first, then launches the thread; when the thread cannot start, the socket held in a local is closed by its destructor before the exception leaves `start()`; both move into the members only once both succeeded.
