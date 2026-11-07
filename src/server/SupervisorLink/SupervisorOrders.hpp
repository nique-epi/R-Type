#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include <string>

namespace rtype::server {

/** @returns Whether @p datagram holds a Disconnection, the order to stop the
 * game process. */
[[nodiscard]] bool holdsStopOrder(std::span<const std::byte> datagram);

/**
 * @returns The nickname the first ExpulsionOrder of @p datagram names; nothing
 * when it holds none, does not read as a datagram of this protocol and
 * version, or when the order does not decode. The supervisor puts one order in
 * each datagram (see kick). Throws nothing: a datagram of the supervisor is
 * trusted to be well formed, and one that is not is dropped.
 */
[[nodiscard]] std::optional<std::string> expulsionIn(
    std::span<const std::byte> datagram);

}  // namespace rtype::server
