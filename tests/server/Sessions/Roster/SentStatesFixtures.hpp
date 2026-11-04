#pragma once

#include <cstdint>
#include <vector>
#include "PacketHeader.hpp"
#include "SentState.hpp"

namespace rtype::server::testing {

/**
 * @returns A state of @p tick sent in the datagrams of @p sequences, with
 * no entity.
 */
SentState sentStateOf(std::uint32_t tick, std::vector<std::uint16_t> sequences);

/**
 * @returns The header of a datagram that acknowledges @p newest and, by the
 * bits of @p earlier, the datagrams before it.
 */
network::PacketHeader acknowledging(std::uint16_t newest,
                                    std::uint32_t earlier);

}  // namespace rtype::server::testing
