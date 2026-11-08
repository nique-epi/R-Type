#pragma once

#include <cstdint>
#include "GameRules.hpp"
#include "HostedDifficulties.hpp"

namespace rtype::server::testing {

/** @returns The difficulties of the shipped waves file: "easy", "hard" and
 * "normal". */
[[nodiscard]] HostedDifficulties shippedDifficulties();

/** @returns The default rules, but for @p lives. */
[[nodiscard]] network::GameRules rulesWithLives(std::uint8_t lives);

}  // namespace rtype::server::testing
