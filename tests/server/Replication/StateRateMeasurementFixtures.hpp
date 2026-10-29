#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>
#include "Endpoint.hpp"
#include "OutgoingDatagram.hpp"

namespace rtype::server::testing {

/**
 * @returns The bytes of the datagrams in @p sent that go to @p player.
 */
[[nodiscard]] std::size_t bytesSentTo(
    std::span<const network::OutgoingDatagram> sent,
    const network::Endpoint& player);

/**
 * @returns The tick each state in @p sent that goes to @p player carries, in
 * order.
 */
[[nodiscard]] std::vector<std::uint32_t> ticksSentTo(
    std::span<const network::OutgoingDatagram> sent,
    const network::Endpoint& player);

/**
 * @returns @p ticks without the ones a network that loses one datagram in
 * ONE_LOST_IN drops, the losses picked by LOSS_SEED.
 */
[[nodiscard]] std::vector<std::uint32_t> ticksThatSurvive(
    const std::vector<std::uint32_t>& ticks);

/**
 * @returns The longest run of ticks, from tick 0 to @p lastTick, with no state
 * in @p ticks: the longest time the player's screen is not updated.
 */
[[nodiscard]] std::uint32_t longestGapInTicks(
    const std::vector<std::uint32_t>& ticks, std::uint32_t lastTick);

}  // namespace rtype::server::testing
