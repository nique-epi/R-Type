#pragma once

#include <cstdint>
#include <optional>
#include "BossStatus.hpp"
#include "EntityIdConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief What the client knows of the fight with the boss, as BossTracker
 * keeps it; the times are on the clock of BossPresentation.
 */
struct BossFight {
  network::BossStage stage{network::BossStage::Arriving};
  std::uint32_t bossEntityId{identifiers::NO_ENTITY_ID};
  std::uint16_t health{};
  std::uint16_t fullHealth{};
  /** @brief The health before the losses of the current lag. */
  std::uint16_t lagHealth{};
  /** @brief When the first status of a boss in play arrived: the bar
   * appears then. */
  std::optional<engine::Duration> fightingSince;
  /** @brief When the newest loss arrived. */
  std::optional<engine::Duration> lastLossAt;
  /** @brief When the health first reached half or less: the first gauge
   * shatters then, unless it already was when the bar appeared. */
  std::optional<engine::Duration> phaseOneEmptiedAt;

  bool operator==(const BossFight&) const = default;
};

}  // namespace rtype::client
