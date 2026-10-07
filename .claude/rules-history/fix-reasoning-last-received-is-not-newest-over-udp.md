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
