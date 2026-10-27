---
description: "A module's install() prepares and keeps nothing open or registered when it returns; what must live until stop() (socket, thread, open file, event subscription, timer) is opened in start()"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: A module's `install()` keeps nothing open — what must live until `stop()` is opened in `start()`

Why: `Application` stops only the modules whose `start()` returned, so a module installed but never started (every module listed after one whose `start()` threw) is never stopped, and whatever its `install()` kept open is never released. Today a failed start is expected to end the program, but nothing in `Application` requires it.

## Rule

1. **`install()` prepares and checks; when it returns, nothing it opened is still open or registered.** It may open and close within the call: read a file through a local `std::ifstream` and parse it, as `WaveCatalog::fromFile` reads `waves.toml`; build the objects the module owns; check a configuration and throw when it is wrong. The doc comment of `IModule::install()` says "opens nothing": read it as "keeps nothing open".
2. **Everything that must live until `stop()` is opened or registered in `start()`**:

   | In `install()` | In `start()` |
   |---|---|
   | read and parse a file, then close it | bind a socket |
   | build the objects the module owns | start a thread |
   | check a configuration, throw when it is wrong | keep a file open (a journal) |
   | | subscribe to an `EventBus` |
   | | schedule a timer |

3. **An event subscription is made in `start()`, never in `install()`.** `SubscriptionHandle` is only a number: destroying it unsubscribes nothing, so a subscription made by a module that is never stopped stays on the bus, with a callback that may point into the destroyed module. Subscribing in `start()` loses no event: the bus delivers only in `dispatch()`, which the game loop calls after its systems, so after `Application::start()` has returned.
4. **Before writing a module, list each resource it holds and the step that opens it**, against the table of point 2. A resource `install()` leaves open when it returns is the defect, whatever the order of the modules today.

## Example

- ❌ **Before (wrong)**: `install()` binds the UDP socket and subscribes to `Collision`; the module listed before it throws from `start()` → `Application` stops nothing of this module, which was installed but never started: the port stays bound and the callback stays on the bus.
- ✅ **After (right)**: `install()` reads and checks the configuration; `start()` binds the socket and subscribes, keeping the `SubscriptionHandle` in a member; `stop()` unsubscribes and closes the socket.
