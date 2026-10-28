# History: a module's install() keeps nothing open

## Context
Pull request #124 added `IModule` and `Application` to `rtype_engine`: an application installs every module in list order, then starts every module, and stops the started ones in the reverse order. The doc comment of `IModule::install()` says it "prepares the module and opens nothing". An analysis of the pull request, read by the user, found that wording too strict and incomplete, and the user asked for rules that tell `install()` and `start()` apart.

## Mistake
No defect was shipped; the rule was asked for before any module was written. Two readings were left open: whether `install()` may read a file at all (the server already reads `waves.toml` once, then closes it), and where a module subscribes to the event bus.

## Root cause
- "Opens nothing" names the wrong criterion: what matters is what is still open when `install()` returns, not whether something was opened and closed during it.
- `Application` stops only the modules whose `start()` returned (`Application::stop()` walks `startedCount_`). Every module listed after one whose `start()` threw was installed and is never stopped, so anything its `install()` kept open is never released.
- `SubscriptionHandle` is a plain struct holding a number: nothing unsubscribes when it goes away. A subscription made in `install()` by a module that is never stopped stays on the bus. `EventBus` delivers only in `dispatch()`, called by the game loop, so subscribing in `start()` loses nothing.

## Rule

1. **`install()` prepares and checks; when it returns, nothing it opened is still open or registered.** It may open and close within the call: read a file through a local `std::ifstream` and parse it, as `WaveCatalog::fromFile` reads `waves.toml`; build the objects the module owns; check a configuration and throw when it is wrong. The doc comment of `IModule::install()` says "opens nothing": read it as "keeps nothing open".
2. **Everything that must live until `stop()` is opened or registered in `start()`**: a socket, a thread, a file kept open, an `EventBus` subscription, a timer.
3. **An event subscription is made in `start()`, never in `install()`.**
4. **Before writing a module, list each resource it holds and the step that opens it.**

## Example

- ❌ **Before (wrong)**: `install()` binds the UDP socket and subscribes to `Collision`; the module listed before it throws from `start()` → `Application` stops nothing of this module, which was installed but never started: the port stays bound and the callback stays on the bus.
- ✅ **After (right)**: `install()` reads and checks the configuration; `start()` binds the socket and subscribes, keeping the `SubscriptionHandle` in a member; `stop()` unsubscribes and closes the socket.
