"""Builds the Huffman code lengths of the server-to-client datagram bodies.

Usage: python3 train_table.py <capture file>... > HuffmanTableConstants.hpp

The captures come from capture_corpus.py. Only the datagrams the server sends
with a session token are read, as only those are compressed. Every byte value
keeps a code (each count starts at 1), so any body can be coded. The lengths
are limited to 15 bits with the package-merge algorithm (Larmore and
Hirschberg), as DEFLATE limits its codes (RFC 1951, section 3.2.2).
"""
import struct
import sys

HEADER_SIZE = 22
SYMBOL_COUNT = 257
END_OF_BODY_SYMBOL = 256
MAXIMUM_CODE_LENGTH = 15
SERVER_TO_CLIENT = 1
TOKEN_OFFSET = 6
RECORD_PREFIX = ">dBBH"
RECORD_PREFIX_SIZE = struct.calcsize(RECORD_PREFIX)


def read_bodies(path):
    with open(path, "rb") as capture:
        data = capture.read()
    position = 0
    while position < len(data):
        _, direction, _, size = struct.unpack_from(RECORD_PREFIX, data, position)
        position += RECORD_PREFIX_SIZE
        datagram = data[position:position + size]
        position += size
        token = struct.unpack_from(">Q", datagram, TOKEN_OFFSET)[0] if size >= HEADER_SIZE else 0
        if direction == SERVER_TO_CLIENT and size > HEADER_SIZE and token != 0:
            yield datagram[HEADER_SIZE:]


def package_merge(counts, limit):
    leaves = sorted((count, (symbol,)) for symbol, count in enumerate(counts))
    current = leaves
    for _ in range(limit - 1):
        packages = [
            (current[index][0] + current[index + 1][0], current[index][1] + current[index + 1][1])
            for index in range(0, len(current) - 1, 2)
        ]
        current = sorted(leaves + packages)
    lengths = [0] * len(counts)
    for _, symbols in current[:2 * len(counts) - 2]:
        for symbol in symbols:
            lengths[symbol] += 1
    return lengths


def kraft_sum_is_one(lengths):
    return sum(2 ** (MAXIMUM_CODE_LENGTH - length) for length in lengths) == 2 ** MAXIMUM_CODE_LENGTH


def main(paths):
    counts = [1] * SYMBOL_COUNT
    bodies = 0
    for path in paths:
        for body in read_bodies(path):
            bodies += 1
            for byte in body:
                counts[byte] += 1
            counts[END_OF_BODY_SYMBOL] += 1
    lengths = package_merge(counts, MAXIMUM_CODE_LENGTH)
    assert max(lengths) <= MAXIMUM_CODE_LENGTH and min(lengths) >= 1
    assert kraft_sum_is_one(lengths), "the code lengths do not form a complete prefix code"
    rows = [", ".join(str(length) for length in lengths[start:start + 16]) for start in range(0, SYMBOL_COUNT, 16)]
    body = ",\n    ".join(rows)
    print(f"""#pragma once

#include <array>
#include <cstdint>
#include "CompressionConstants.hpp"

namespace rtype::network {{

/**
 * @brief The bit length of the Huffman code of each symbol: the 256 byte
 * values, then the end-of-body symbol. Produced by
 * tests/tools/compression/train_table.py from {bodies} datagram bodies the
 * server sent with a session token in real games; never edited by hand.
 * Both programs hold the same table, so changing it changes the protocol
 * version.
 */
constexpr std::array<std::uint8_t, HUFFMAN_SYMBOL_COUNT>
    SERVER_TO_CLIENT_CODE_LENGTHS{{
    {body}}};

}}  // namespace rtype::network""")


main(sys.argv[1:])
