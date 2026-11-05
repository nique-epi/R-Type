"""Builds the zstd dictionary of the server-to-client datagram bodies.

Usage: python3 train_dictionary.py <capture file>... > assets/compression/server_to_client.dict

The captures come from capture_corpus.py. Only the datagrams the server sends
with a session token are read, as only those are compressed. The training is
the one of the zstd command line (`zstd --train-fastcover`), which must be on
the PATH: on macOS `brew install zstd`, on Debian `apt install zstd`.
"""
import os
import struct
import subprocess
import sys
import tempfile

HEADER_SIZE = 22
SERVER_TO_CLIENT = 1
TOKEN_OFFSET = 6
DICTIONARY_SIZE = 2048
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


def train(paths):
    with tempfile.TemporaryDirectory() as folder:
        samples = os.path.join(folder, "samples")
        os.mkdir(samples)
        count = 0
        for path in paths:
            for body in read_bodies(path):
                with open(os.path.join(samples, "%07d" % count), "wb") as sample:
                    sample.write(body)
                count += 1
        dictionary = os.path.join(folder, "dictionary")
        subprocess.run(["zstd", "--train-fastcover", "-r", samples, "--maxdict=%d" % DICTIONARY_SIZE,
                        "-o", dictionary, "-q"], check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        with open(dictionary, "rb") as result:
            return count, result.read()


def main(paths):
    count, dictionary = train(paths)
    sys.stdout.buffer.write(dictionary)
    print("%d datagram bodies, %d bytes" % (count, len(dictionary)), file=sys.stderr)


main(sys.argv[1:])
