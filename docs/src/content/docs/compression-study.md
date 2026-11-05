---
title: Compression study
description: Which generic compression, if any, the datagrams of the game should use, measured on real games, and why the server sends its datagram bodies as zstd frames.
---

The protocol could shrink its datagrams with a generic compressor. This page records what was measured on datagrams of real games, the decision, and what the measurement does not cover. The format that came out of it is in [Compression](/R-Type/protocol/#compression), the code in [Network](/R-Type/network/#compression-compressingdatagramsocket).

## Method

- **Corpus.** `tests/tools/compression/capture_corpus.py` plays scripted pilots against the real server through the whole protocol (cookie handshake, ready, 60 inputs per second) and records every datagram of both directions. Eight games were recorded, with one to four pilots and different seeds, about 40 seconds each.
- **What a datagram holds.** A `WorldState` datagram from the server weighs 84 bytes on average with one pilot and 126 with four, header included. A `PlayerInput` datagram from a client weighs 30: the 22 bytes of the header, then 8 bytes of message. The header is never coded: it carries the token and the sequence, which are random.
- **Measure.** Each algorithm codes the body of a datagram, the bytes after the header, and the datagram is kept as it is when the result is not shorter. An algorithm that learns (a Huffman table, a dictionary) is trained on the first half of a game, or on four games, and measured on the rest or on games it never saw.

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

The tables of an algorithm that learns were trained on the same game they were measured on. Measured on two games the training never saw, after training on four others:

| Algorithm | Gain on a datagram |
|---|---|
| Huffman with a fixed table | 31.9 % (29.8 % and 32.5 % for the two games), against 32.2 % on the training games |
| zstd, 2 kB dictionary, level 1 | 25.1 % |
| zstd, 2 kB dictionary, level 3 | 23.9 % |
| zstd, dictionaries of 2 to 16 kB, levels 1 and 3, the other settings | 22.5 % to 24.8 % |

The zstd dictionaries were trained by the zstd command line (`--train-fastcover` or `--train-cover`) on the bodies of the four games, and the frames were written without checksum, content size or dictionary identifier. A zstd frame adds a header and a block header to every datagram, which a body of about 100 bytes cannot absorb; coding takes about 0.5 µs and decoding 0.2 µs.

## Decision

The server sends the body of its datagrams as a zstd frame made with a 2 kB dictionary trained on real games, at level 1. zstd is an established library with its own decoder, so the project keeps no coding or decoding algorithm of its own, where the Huffman code would have been about 60 lines of ours to maintain. The price is a gain of about 25 % instead of the 32 % the Huffman code was measured at.

The gain is about 1.1 kB/s per pilot in a game of four, about 9 kbit/s. It is small next to a link, and real next to the 11 kbit/s the 22-byte header costs. Packing the fields of the messages tighter, or sending only what changed, shrinks the datagrams before any compressor sees them, and is measured separately.

The dictionary is part of the protocol: both programs hold the same bytes, and a frame does not name it, so changing it changes the version. `tests/tools/compression/train_dictionary.py` builds it again from new captures. zstd is dual licensed (BSD-3-Clause or GPL-2.0-only); the project uses it under the BSD terms.

## What the measure does not cover

- The pilots are scripted and move at random, so waves, missiles and kills are fewer than in a played game. Other games can give another gain.
- Every game was recorded on one machine, over the loopback, with the same build of the server.
- Times are those of a warm loop on a laptop, one datagram at a time. The zlib figures include setting up a stream for each datagram; the zstd figures reuse one context.
- The coded datagrams were not run through the network simulator at a loss rate in this measure; the tests check that a coded datagram decodes alone.
