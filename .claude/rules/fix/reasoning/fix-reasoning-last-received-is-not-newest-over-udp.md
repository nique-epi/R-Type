---
description: Over UDP the last datagram received is not the newest one sent; a "last one wins" rule needs an ordering key the receiver checks
trigger: always_on
---

# RULE: Over UDP, the last datagram received is not the newest one sent

## Rule

1. **A rule of the form "the last X received wins"** (screen to show, ready state, phase, position) **needs an ordering key the receiver compares**: a sequence number, a tick, a revision. Without it, a late datagram undoes a newer one.
2. **Walk a late duplicate through every state rule before writing it**: an older copy of each message arriving after the newer one. Write what the receiver shows or does then.
3. **A wrapping counter is compared with serial number arithmetic** (RFC 1982), never a plain `>`: newer means ahead by at least 1 and by less than half the range, 1 to 32767 for 16 bits. Exactly half the range (32768) is undefined in RFC 1982, so it is never newer.
4. **The ordering check runs after the authenticity check**, so a forged datagram cannot move the counter and make every genuine one look old.
5. **Repetition is not ordering**: a message repeated every 500 ms corrects a loss, not a reordering; both properties are stated separately.

### A decision on what a message leaves out waits until every part of it arrived

1. **Before a receiver acts on an absence** (an entity a state does not list, a player a lobby does not name), find whether that message can come in several parts with the same key (`WorldState` split by `WorldStateCodec::encode`, all parts with one `tick`). A part shows what it holds, never what the whole message leaves out.
2. **Judge the absence on a complete message**: when the parts carry no count, a message is known complete once a part with a newer key arrives; act then, on the previous key. Say in the docs what this costs in delay (one more interval).
3. **Walk a split message through the rule before writing it**: the first part leaves the item out, the second lists it. Write what the receiver does at each part; an item removed then created again (a second effect, a second sound) is the symptom.

## Example

- ❌ **Before (wrong)**: "the client shows the screen of the last repeated message it received" and "version 0 receivers may ignore `sequence`" → a `LobbyState` arriving late during a game brings the lobby back on screen.
- ✅ **After (right)**: within a session, a receiver drops a datagram that is not newer than the newest one it accepted from that peer (16-bit `sequence`, RFC 1982), checked after the token.
- ❌ **Before (wrong)**: the client removed an entity no `WorldState` had listed for 54 ticks as soon as a part of a state left it out; a later part of the same tick that listed it created it again, with its appearance effect a second time.
- ✅ **After (right)**: the state of a tick is judged once a part of a newer tick shows it complete; the entity goes 950 ms after its last listing instead of 900 ms, and the docs say so.
