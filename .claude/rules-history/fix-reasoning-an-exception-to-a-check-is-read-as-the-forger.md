# History: an exception to a check is walked through as the forger

## Context

Version 0 of the network protocol. The user asked for the session token to be checked from version 0. A client has no token before its connection is accepted, so its `ConnectionRequest` had to carry 0.

## Mistake

I wrote the exception as "a `ConnectionRequest` may carry 0", justified by "the answer goes to the address of the player, never to whoever forged it". That only covered the token leaking. It did not cover what the request itself makes the server do: give a player slot to a forged address, and, since any datagram kept a session alive, keep that slot for as long as the forged requests keep coming. The pull request review found that four forged addresses fill the game for good.

## Root cause

I checked the exception from the point of view of the honest client and of an attacker trying to read the token, never from the point of view of an attacker who does not need to read anything: one who only sends, repeatedly, from forged addresses.

## Rule

Before writing an exception to a check, play the forger step by step (forged address, repeated as often as wanted, answers never read) and write what each step makes the receiver allocate, reserve, keep alive or send; if anything is reserved or kept, require a proof of reachability first or reserve nothing; check every triggered answer against the anti-amplification rule.

## Example

- ❌ **Before (wrong)**: "a `ConnectionRequest` may carry token 0" → forged requests take the four slots and keep them.
- ✅ **After (right)**: a stateless cookie challenge before any slot is given; only a datagram carrying the token keeps a session alive.
