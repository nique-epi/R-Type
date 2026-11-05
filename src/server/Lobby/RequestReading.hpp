#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include "ConnectionRequest.hpp"

namespace rtype::server {

/**
 * @returns The first ConnectionRequest of @p datagram, or nothing when the
 * datagram is not of this protocol or holds no request.
 * @throws A NetworkException when the datagram is shorter than a header, its
 * messages do not split, or its first request is refused by its reader.
 */
[[nodiscard]] std::optional<network::ConnectionRequest> firstConnectionRequest(
    std::span<const std::byte> datagram);

/** @returns Whether @p datagram starts with a header of this protocol and
 * another version. */
[[nodiscard]] bool isOfAnotherVersion(std::span<const std::byte> datagram);

}  // namespace rtype::server
