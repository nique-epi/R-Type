#pragma once

#include <optional>
#include "ConnectionRefused.hpp"
#include "GameRules.hpp"
#include "HostedDifficulties.hpp"

namespace rtype::server {

/**
 * @returns Why the supervisor refuses to open a game with @p rules: a
 * difficulty @p difficulties does not hold first (UnknownDifficulty), then
 * lives out of LOWEST_LIVES to HIGHEST_LIVES (LivesOutOfRange); nothing when
 * it may open it. The wire already keeps friendly fire to 0 or 1.
 */
[[nodiscard]] std::optional<network::RefusalReason> refusalOf(
    const network::GameRules& rules, const HostedDifficulties& difficulties);

}  // namespace rtype::server
