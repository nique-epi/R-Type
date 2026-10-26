# History: over UDP, the last datagram received is not the newest one sent

## Context

Version 0 of the network protocol has no guaranteed messages. To recover from losses, each phase has a message the server repeats (`LobbyState`, `WorldState`, `GameEnded`), and the client shows the screen of the last one it received. The header carries a 16-bit `sequence`, which version 0 receivers were allowed to ignore.

## Mistake

The recovery rule only considered losses. UDP also reorders: a `LobbyState` sent just before the game started can arrive after the first `WorldState` and bring the lobby back; a late `GameEnded` can bring the end screen back; a late `ReadyState` can undo the player's latest choice. The pull request review found it.

## Root cause

I designed the self-correction against one property of UDP (loss) and wrote "last received" as if it meant "newest sent". The ordering field was already in the header and was explicitly left unused.

## Rule

A "last one wins" rule needs an ordering key the receiver compares; walk a late duplicate through every state rule; compare wrapping counters with serial number arithmetic; check ordering after authenticity; state repetition and ordering as separate properties.

## Example

- ❌ **Before (wrong)**: "version 0 receivers may ignore `sequence`" next to "the client shows the screen of the last repeated message it received".
- ✅ **After (right)**: a receiver drops a datagram not newer than the newest one accepted from that peer in the session, after the token check.

## Update (2026-10-24) — an absence judged on one part of a split state

### Context
The client removes an entity no `WorldState` listed for 900 ms (54 ticks), after its three `EntityDestroyed` were all lost. A state too big for one datagram is sent in several parts with the same `tick`.

### Mistake
The first version of the removal judged every part as it arrived: when a part of tick T left out an entity last listed 54 ticks before, the entity was removed, even though a later part of the same tick listed it; that part then created it again, with its appearance effect a second time. The plan had walked the parts through the age of an entity (a part refreshes what it lists) but not through the decision to remove one. A review found it.

### Root cause
A part shows what it holds; it says nothing of what the whole state leaves out. The existing rule asked to walk late duplicates through every state rule, not split messages through rules that act on an absence.

### What was done
`LastListedTicks` judges the state of a tick once a part of a newer tick closes that tick. A test plays a state in two parts (the first leaving the entity out, the second listing it) and fails on the first version; another fails if a second part of a tick completes its own state. The protocol and client pages give the new delay, 950 ms after the last listing with a state every 50 ms.
