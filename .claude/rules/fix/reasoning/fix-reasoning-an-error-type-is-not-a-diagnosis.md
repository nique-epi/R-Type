---
description: A branch that destroys state triggers on the precise error codes that prove the condition, never on an error type
trigger: always_on
---

# RULE: An error type is not a diagnosis

## Rule

Before writing a branch that **destroys state** (disconnecting a client, closing a game, clearing a queue, resetting a session):

1. **Narrow the trigger to precise codes**, never to the type. List the codes that prove the condition (`asio::error::connection_refused`, `asio::error::operation_aborted` on shutdown…); **everything else is transient by default**. The default is always "keep the state": being wrong in that direction costs a retry, in the other a kicked player.
2. **Read the default behavior of the destructive call** in the docs or headers before writing it (what does `close()` do to pending operations? which handlers receive `operation_aborted`?), and make the choices explicit at the call site.
3. **Check what the layer below already does** before adding handling on top.
4. **Never assert an API behavior in a doc comment without having checked it.**

## Example

- ❌ **Before (wrong)**: `if (error) { disconnectClient(clientId); }` in the UDP receive handler — a transient `connection_refused` (ICMP from another port) kicks the player.
- ✅ **After (right)**: ignore `operation_aborted` (intentional shutdown), restart receiving on transient errors, and only disconnect a client when its inactivity timeout expires — UDP has no connection to lose.
