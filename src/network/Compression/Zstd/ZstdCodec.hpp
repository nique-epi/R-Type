#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include <vector>

namespace rtype::network {

/**
 * @brief Compresses a datagram body as one zstd frame, with the dictionary of
 * the protocol and without checksum, content size or dictionary identifier
 * (see ZstdCodecConstants.hpp).
 *
 * Nothing is kept between two calls but one zstd context per thread, so it can
 * be called from any thread.
 *
 * @returns The frame, which can be longer than @p body (its caller then keeps
 * the original), or std::nullopt when zstd fails.
 * @throws CompressionUnavailableException when zstd cannot allocate its
 * context or the dictionary.
 */
[[nodiscard]] std::optional<std::vector<std::byte>> compressBody(
    std::span<const std::byte> body);

/**
 * @brief Reads back a body compressed by compressBody().
 *
 * The output buffer is @p maximumSize bytes, so a frame announcing more, or
 * made to expand, gets an error and nothing is allocated from a value of the
 * frame. A datagram a stranger sent can be given to it.
 *
 * @returns The body, or std::nullopt when @p coded is not exactly one zstd
 * frame that decodes to at most @p maximumSize bytes: it is cut, corrupt, or
 * has bytes after its end. A frame made with another dictionary may decode to
 * other bytes: the frame names no dictionary and has no checksum.
 * @throws CompressionUnavailableException when zstd cannot allocate its
 * context or the dictionary.
 */
[[nodiscard]] std::optional<std::vector<std::byte>> decompressBody(
    std::span<const std::byte> coded, std::size_t maximumSize);

}  // namespace rtype::network
