---
description: Over UDP the last datagram received is not the newest one sent; a "last one wins" rule needs an ordering key the receiver checks
trigger: always_on
---

# RULE: Over UDP, the last datagram received is not the newest one sent

## Rule

1. **A rule of the form "the last X received wins"** (screen to show, ready state, phase, position) **needs an ordering key the receiver compares**: a sequence number, a tick, a revision. Without it, a late datagram undoes a newer one.
2. **Walk a late duplicate through every state rule before writing it**: an older copy of each message arriving after the newer one. Write what the receiver shows or does then.
3. **A wrapping counter is compared with serial number arithmetic** (RFC 1982): newer means ahead by 1 to half the range, never a plain `>`.
4. **The ordering check runs after the authenticity check**, so a forged datagram cannot move the counter and make every genuine one look old.
5. **Repetition is not ordering**: a message repeated every 500 ms corrects a loss, not a reordering; both properties are stated separately.

## Example

- ❌ **Before (wrong)**: "the client shows the screen of the last repeated message it received" and "version 0 receivers may ignore `sequence`" → a `LobbyState` arriving late during a game brings the lobby back on screen.
- ✅ **After (right)**: within a session, a receiver drops a datagram that is not newer than the newest one it accepted from that peer (16-bit `sequence`, RFC 1982), checked after the token.
