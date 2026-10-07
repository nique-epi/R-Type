---
description: Every exception to an authentication or validation check is walked through as someone who forges the source address, repeats, and never reads the answers
trigger: always_on
---

# RULE: An exception to a check is walked through as the forger before it is written

## Rule

Before writing an exception to an authentication or validation check (a field allowed to be 0, a message accepted from an unknown sender, a request answered again, a datagram that refreshes a timer):

1. **Play the forger, step by step**: someone who forges the source address, sends the excepted message as often as they like, and never receives the answers. Write what each step makes the receiver allocate, reserve, keep alive or send.
2. **Repeat the step**: an exception that is harmless once is often a denial of service when repeated every second (a slot kept, a timer refreshed, a reply sent each time).
3. **If the forger can make the receiver reserve or keep anything**, the exception needs a proof of reachability first (a stateless challenge the real owner of the address answers), or it must reserve nothing.
4. **Check the sizes against the anti-amplification rule** for every answer the exception triggers.

## Example

- ❌ **Before (wrong)**: "a `ConnectionRequest` may carry token 0, since the client has no token yet" → a forged address gets a player slot, and the same request repeated every second keeps the session alive: four forged addresses fill the game for good.
- ✅ **After (right)**: the server answers a tokenless request with a stateless cookie and gives a slot only to a request carrying it; a request never keeps a session alive, only a datagram carrying the token does.
