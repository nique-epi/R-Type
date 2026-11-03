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

### A figure kept per value of a byte is walked as the forger too

1. **Before keeping a figure per value a sender writes** (a type byte, a kind, a reason), write what one forged datagram adds to everything built from those figures: the memory, the copy under a lock, and any output repeated afterwards, such as a line written every second.
2. **Count only the values the protocol names, and fold the rest into one figure.** Bounding the memory is not enough: a total never goes back to zero, so a line written once a second for each figure grows for the rest of the run.

## Example

- ❌ **Before (wrong)**: "a `ConnectionRequest` may carry token 0, since the client has no token yet" → a forged address gets a player slot, and the same request repeated every second keeps the session alive: four forged addresses fill the game for good.
- ✅ **After (right)**: the server answers a tokenless request with a stateless cookie and gives a slot only to a request carrying it; a request never keeps a session alive, only a datagram carrying the token does.
- ❌ **Before (wrong)**: traffic figures in a table of 256 entries, one per type byte, each non-zero one a line of the file every second → one forged datagram of 790 bytes adds 256 lines per second, 36 MB per hour, for the rest of the run.
- ✅ **After (right)**: one slot per message of the protocol and one for every other byte: the same datagram adds 19 lines.
