#pragma once

#include <cstdint>
#include <vector>
#include "IncomingDatagram.hpp"

/** @returns A datagram from the player at index 0 of the server tests,
 * holding a ServerInformationRequest sent at @p sentAt, that arrived at
 * REQUEST_ARRIVAL. */
[[nodiscard]] rtype::network::IncomingDatagram informationRequestArriving(
    std::uint32_t sentAt);

/** @returns @p datagram with its last byte left out. */
[[nodiscard]] rtype::network::IncomingDatagram cutShort(
    rtype::network::IncomingDatagram datagram);
