#pragma once

#include <cstdint>
#include <optional>
#include "BetsReceipt.hpp"

namespace rtype::client {

/** @returns The chips the player may lay on the round in progress: once the
 * master took bets of the round (@p accepted), the chips it left and those
 * it holds on the cloth; @p walletChips before. */
[[nodiscard]] std::uint32_t chipsAtHandOf(
    std::uint32_t walletChips,
    const std::optional<network::BetsReceipt>& accepted);

}  // namespace rtype::client
