---
description: A library object may report a failure in its state instead of throwing; read the constructor before wrapping it as a check
trigger: always_on
---

# RULE: A library may fail without throwing — read every return of the constructor before trusting a try/catch around it

Why: `requireMasterUrl` refused a bad master url by catching what `MasterClient` throws; cpp-httplib throws only for a scheme, and for a port outside 1 to 65535 it builds an empty `Client`, whose first call dereferenced a null pointer: the check meant to refuse the url crashed the client.

## Rule

1. **Before writing a check, a wrapper or a `try`/`catch` around a library constructor or parser**, read its definition in the headers vcpkg installed (`build/vcpkg_installed/<triplet>/include/`), and list each early `return`, each `throw` and each state it leaves (`is_valid()`, `good()`, an empty member, `operator bool`).
2. **Every way it reports a failure is turned into the project's exception at that wrapper**: a `throw` caught, and a state checked right after building (`if (!client->is_valid()) { throw InvalidMasterUrlException(url); }`). A catch alone covers only the failures the library throws.
3. **The test of the check feeds one input for each way of failing**: a value the library throws for and a value it only marks invalid. The red proof removes the state check: the second test must fail.
4. **A validator built on such a wrapper inherits its blind spots**: when a new validator reuses an existing wrapper, read that wrapper with point 1 instead of trusting its doc comment.

## Example

- ❌ **Before (wrong)**: `clientOf()` returned `std::make_unique<httplib::Client>(url)` inside `catch (const std::invalid_argument&)`; `requireMasterUrl("http://127.0.0.1:0")` built it, `set_keep_alive()` dereferenced a null `cli_`, and the client stopped on SIGSEGV.
- ✅ **After (right)**: `clientOf()` also throws `InvalidMasterUrlException` when `!client->is_valid()`; `AUrlWithAPortPastTheLastIsRefused` and `RefusesAPortOutsideTheRange` crash without that check and pass with it.
