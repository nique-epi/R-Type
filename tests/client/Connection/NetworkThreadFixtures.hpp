#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "NetworkThread.hpp"

/** @returns The loopback address on @p port. */
[[nodiscard]] rtype::network::Endpoint loopbackOnPort(std::uint16_t port);

/** @returns EXCHANGED_BYTES, as the payload of a datagram. */
[[nodiscard]] std::vector<std::byte> exchangedBytes();

/**
 * @brief Drains @p networkThread the way the frame loop does, until something
 * arrived or DATAGRAM_TIMEOUT passed. A test that waits for the error it
 * throws ignores what it returns.
 * @returns What arrived; nothing after DATAGRAM_TIMEOUT.
 */
std::vector<rtype::network::IncomingDatagram> drainUntilReceived(
    rtype::client::NetworkThread& networkThread);
