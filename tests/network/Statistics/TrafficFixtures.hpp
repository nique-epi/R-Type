#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>
#include "DirectionTraffic.hpp"
#include "Endpoint.hpp"
#include "MessageType.hpp"
#include "TrafficFigures.hpp"

namespace rtype::network::testing {

/** @returns A datagram of this protocol holding one message of @p type, with
 * @p payloadSize bytes of payload. */
[[nodiscard]] std::vector<std::byte> datagramHolding(MessageType type,
                                                     std::size_t payloadSize);

/** @returns @p datagram with one more message of @p type at its end. */
[[nodiscard]] std::vector<std::byte> withMessage(
    std::vector<std::byte> datagram, MessageType type, std::size_t payloadSize);

/** @returns The loopback address on @p port. */
[[nodiscard]] Endpoint loopbackOnPort(std::uint16_t port);

/** @returns What @p traffic counted for the messages of @p type. */
[[nodiscard]] TrafficFigures figuresOf(const DirectionTraffic& traffic,
                                       MessageType type);

/** @returns What @p file holds. */
[[nodiscard]] std::string textOf(const std::filesystem::path& file);

}  // namespace rtype::network::testing
