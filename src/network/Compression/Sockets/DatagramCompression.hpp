#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include <vector>

namespace rtype::network {

/**
 * @brief The datagram with its body as a zstd frame and COMPRESSED_FLAG
 * raised.
 *
 * Only a datagram of a session is coded: a datagram sent before one (a
 * ConnectionRequest, a ConnectionChallenge, a ConnectionRefused) carries the
 * token 0 and stays readable by a peer of any version, with the size the
 * handshake rules count on. A coded body that is not shorter than the original
 * is not worth sending.
 *
 * @returns The coded datagram, or std::nullopt when @p datagram is to be sent
 * as it is: no body, token 0, a flag already raised, or no gain.
 * @throws CompressionUnavailableException when zstd cannot allocate what it
 * needs.
 */
[[nodiscard]] std::optional<std::vector<std::byte>> compressedForm(
    std::span<const std::byte> datagram);

/**
 * @returns Whether @p datagram carries the protocol id and the version of this
 * code with COMPRESSED_FLAG raised. Reads the header only.
 */
[[nodiscard]] bool isCompressed(std::span<const std::byte> datagram);

/**
 * @brief The datagram a sender coded, with its original body and the flag
 * cleared.
 *
 * The body never grows past MAXIMUM_BODY_SIZE, so the result fits in
 * MAX_DATAGRAM_SIZE whatever @p datagram holds.
 *
 * @pre isCompressed(@p datagram).
 * @returns The plain datagram, or std::nullopt when the token is 0 (nobody
 * codes a datagram before a session) or the body is not exactly one coded
 * body.
 * @throws CompressionUnavailableException when zstd cannot allocate what it
 * needs.
 */
[[nodiscard]] std::optional<std::vector<std::byte>> plainForm(
    std::span<const std::byte> datagram);

}  // namespace rtype::network
