---
description: A new call to an existing helper keeps the guards that all its current callers carry
trigger: always_on
---

# RULE: A new call to an existing helper keeps the guards its callers already carry

## Rule

Before adding a call to an existing function — especially a private one, especially in a type that orchestrates asynchronous state (network session, game, game loop):

1. **Read all current callers** (`grep -rn`) and note what each one checks **before** the call. A guard that all of them carry is a precondition of the helper, even if nothing writes it down.
2. **Keep it**, or write in the helper's doc comment why the new site is exempt.
3. Ask **when** the new site runs compared to the others (before the handshake completes? during server shutdown? from another thread?).
4. If the precondition deserves to be guaranteed rather than repeated, move it **into** the helper — checking that no caller relied on its absence.

## Example

- ❌ **Before (wrong)**: `sendSnapshot(clientId)` called from the new reconnection handler, without the `if (session.isHandshakeComplete())` the two other callers carry → snapshot sent to a client that does not know its id yet.
- ✅ **After (right)**: the same guard, or the check moved into `sendSnapshot` with an explicit return.
