# A wrapper put on a socket for one kind of peer serves every peer of that socket

## Context

`CompressingDatagramSocket` was added around the socket of the server to compress what it sends to the players. Games then moved into processes of their own, and each game started sending its status to its supervisor through that same socket, under a session token.

## Mistake

`compressedForm()` coded every datagram of a session that got shorter. From the first measures of a game, 10 s after its start, zstd shortened the status of some games. The socket of the supervisor drops what it receives compressed and counts nothing, so it never answered, and after 5 s without an answer the game stopped with "the supervisor is silent" and closed the sessions of its players. Eight to sixteen of 32 load-test bots were cut in each run.

## Root cause

The wrapper was written for one receiver, the client, which decodes. The rule that a server never decodes was right for players, and nobody listed the new datagrams that a later change sent through the same socket to another server. The unit tests of the wrapper only saw the peer it was written for, and the drop left no counter or journal line.

## Rule

1. **Before wrapping a socket, list everything sent through it and who reads it**: `git grep -n 'queueDatagram(' -- src`.
2. **For each receiver, write what it does with the changed datagram.** A receiver that drops it silently is the defect.
3. **Run the whole program once with real traffic**: a test of the wrapper alone sees only the peer it was written for.

## Example

- ❌ **Before (wrong)**: `compressedForm()` codes every datagram of a session, the status of a game included.
- ✅ **After (right)**: `SUPERVISOR_LINK_MESSAGES` stay plain, and a test asks the compressed form of a status.
