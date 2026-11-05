#pragma once

namespace rtype::network {

/**
 * @brief The zstd level the server compresses a body at: the fastest. On the
 * real games it also gave the smallest datagrams (25.1 % against 23.9 % at
 * level 3), the bodies being too short for a deeper search to find more.
 */
constexpr int COMPRESSION_LEVEL = 1;

/**
 * @brief The value that leaves an option out of a zstd frame header: the size
 * of the content and the identifier of the dictionary. Both programs hold the
 * dictionary, and the size is the one of the datagram. The checksum of a frame
 * is off by default in zstd; the datagram has its own.
 */
constexpr int FRAME_OPTION_OFF = 0;

}  // namespace rtype::network
