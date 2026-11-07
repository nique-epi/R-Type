# RULE: A library may fail without throwing — read every return of the constructor before trusting a try/catch around it

## Context

RTY-240 gave the client a `master-url` setting. Its validator, `requireMasterUrl`, builds a `network::MasterClient` and turns the `InvalidMasterUrlException` it throws into a `CommandRefusedException`, so `set master-url` refuses a url the client cannot call. `MasterClient` wraps `httplib::Client` through `clientOf()`, which caught `std::invalid_argument`. Its doc comment said it refused any url that is not `http://<host>[:<port>]`.

## Mistake

The validator and its tests covered an `https://` url only, the one failure cpp-httplib 0.51.0 throws for. The review of the branch read `Client::Client` (`httplib.h:15416`): for a port `parse_port` refuses (0, or past 65535), the constructor returns early and leaves `cli_` null. `MasterClient`'s constructor then called `client_->set_keep_alive(true)`, which dereferences `cli_`. `--set master-url=http://127.0.0.1:0` stopped the client with SIGSEGV, during the check written to refuse that url. The server's `master-url`, read by the same `clientOf()`, had the same crash before the branch.

## Root cause

The validator was written from the doc comment of the wrapper and from the one failing input known (`https://`), never from the definition of the library constructor. A `try`/`catch` was taken as the whole failure contract, while the library reports part of its failures through the state of the object (`is_valid()`).

## Rule

See the core file of the same name: read the constructor's every `return` and `throw`, check the state after building, test one input per way of failing.

## Example

- ❌ `clientOf()` with a catch only → SIGSEGV on port 0.
- ✅ `clientOf()` checks `is_valid()` → `InvalidMasterUrlException`; two tests, one per failure, red (SEGFAULT) without the check.
