#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include "PredictedCommand.hpp"
#include "PredictedGun.hpp"

namespace rtype::client::testing {

/** @returns The commands numbered 1 to @p count, each holding shoot. */
[[nodiscard]] std::vector<PredictedCommand> commandsHoldingShoot(
    std::size_t count);

/** @returns The numbers of the commands of @p commands that @p gun fires,
 * oldest first. */
[[nodiscard]] std::vector<std::uint32_t> inputsFiredBy(
    PredictedGun& gun, const std::vector<PredictedCommand>& commands);

}  // namespace rtype::client::testing
