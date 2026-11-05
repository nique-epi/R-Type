---
title: Compression study
description: Which generic compression, if any, the datagrams of the game should use, measured on real games, and why the server sends a static Huffman code.
---

The protocol could shrink its datagrams with a generic compressor. This page records what was measured on datagrams of real games, the decision, and what the measurement does not cover. The format that came out of it is in [Compression](/R-Type/protocol/#compression), the code in [Network](/R-Type/network/#compression-compressingdatagramsocket).

## Method

- **Corpus.** `tests/tools/compression/capture_corpus.py` plays scripted pilots against the real server through the whole protocol (cookie handshake, ready, 60 inputs per second) and records every datagram of both directions. Eight games were recorded, with one to four pilots and different seeds, about 40 seconds each.
- **What a datagram holds.** A `WorldState` datagram from the server weighs 84 bytes on average with one pilot and 126 with four, header included. A `PlayerInput` datagram from a client weighs 30: the 22 bytes of the header, then 8 bytes of message. The header is never coded: it carries the token and the sequence, which are random.
- **Measure.** Each algorithm codes the body of a datagram, the bytes after the header, and the datagram is kept as it is when the result is not shorter. The table of an algorithm that learns (Huffman, a dictionary) is trained on the first half of a game and measured on the second half.

## Results

Datagrams from the server to a client in a game of four pilots, with the time to code one datagram on an Apple silicon laptop:

| Algorithm | Gain on a datagram | Time |
|---|---|---|
| RLE | 0.1 % | 285 ns |
| LZ4 | 5.1 % | 220 ns |
| LZ4 level 9 | 6.4 % | 3.2 µs |
| LZ4 with a dictionary | 14.1 % | 404 ns |
| zlib level 1, raw deflate | 16.4 % | 4.4 µs |
| zlib level 6, raw deflate | 18.2 % | 4.7 µs |
| zlib level 6 with a dictionary | 27.2 % | 5.9 µs |
| Huffman with a fixed table | 30.6 % | 18 ns |

With one pilot the order is the same and the gains are smaller: 23.2 % for the Huffman table, 17.3 % for zlib with a dictionary, 2.0 % for LZ4. In the other direction, the 30 bytes of a `PlayerInput` datagram gain 9.8 % with the Huffman table and nothing with the others, which is under 1 kB/s for four pilots, so only the server codes.

The Huffman table above was trained on the same game it was measured on. On six games, four to train and two the table had never seen, the gain on the datagrams of the server is **31.9 %** (29.8 % and 32.5 % for the two games), against 32.2 % on the training games: the table does not stick to one game.

zstd with a dictionary trained on the same four games and measured on the same two reached 22.9 % to 25.0 % of a datagram, for dictionaries of 1 to 16 kB and levels 1, 3 and 9, in 0.5 to 3.5 µs. A zstd frame adds a header and a block header to every datagram, which a body of about 100 bytes cannot absorb.

## Decision

The server sends the body of its datagrams in Huffman code with a fixed table. It wins on gain (about 32 % against 25 % for the best zstd and 27 % for zlib with a dictionary), on time (18 ns), on size of the code (no frame, no dictionary to ship, no dependency) and on safety: the decoder has no state and writes at most 1178 bytes. Its cost is that the table is part of the protocol, so it must be trained again when the bytes of the states change.

The gain is about 1.3 kB/s per pilot in a game of four, about 10 kbit/s. It is small next to a link, and real next to the 11 kbit/s the 22-byte header costs. Packing the fields of the messages tighter, or sending only what changed, shrinks the datagrams before any compressor sees them, and is measured separately.

## What the measure does not cover

- The pilots are scripted and move at random, so waves, missiles and kills are fewer than in a played game. Other games can give another gain.
- Every game was recorded on one machine, over the loopback, with the same build of the server.
- Times are those of a warm loop on a laptop, one datagram at a time. The zlib figures include setting up a stream for each datagram; the zstd figures reuse one context.
- The coded datagrams were not run through the network simulator at a loss rate in this measure; the tests check that a coded datagram decodes alone.
