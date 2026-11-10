---
description: A decorator put on a socket for one peer also changes what every other peer of that socket receives
trigger: always_on
---

# RULE: A wrapper put on a socket for one kind of peer serves every peer of that socket

Why: the server compressed what it sent to players, and the status of a game to its supervisor went through the same socket; the supervisor drops what is compressed, so two to four games of eight stopped.

## Rule

1. **Before wrapping a socket, changing a wrapper or sending something new through a wrapped socket, list everything sent through it and who reads it**: `git grep -n 'queueDatagram(' -- src`.
2. **For each receiver, write what it does with the changed datagram.** A receiver that drops it silently is the defect.
3. **Run the whole program once with real traffic that reaches each of those receivers and meets the conditions that trigger the wrapper**: a test of the wrapper alone sees only the peer it was written for, and a status that zstd does not shorten passes on the faulty code.

## Example

- ❌ **Before (wrong)**: `compressedForm()` codes every datagram of a session, the status of a game included.
- ✅ **After (right)**: `SUPERVISOR_LINK_MESSAGES` stay plain, and a test asks the compressed form of a status.
