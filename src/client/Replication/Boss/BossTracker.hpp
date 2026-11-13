#pragma once

#include <cstdint>
#include <optional>
#include "BossFight.hpp"
#include "BossStatus.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The fight with the boss as the BossStatus of the server tell it:
 * the newest status, the ordered channel handing them over in the order the
 * server sent them, and when the boss stage started.
 */
class BossTracker {
 public:
  /** @brief Takes @p status in, arrived @p now: an Arriving status starts the
   * boss stage over; any other gives the health, and the lag of a loss. */
  void follow(const network::BossStatus& status, engine::Duration now);

  /** @returns The fight as known so far. */
  [[nodiscard]] const BossFight& fight() const;

  /** @brief Forgets the boss, as if no status had come: no bar, no
   * arrival. */
  void forget();

  /** @returns The time since the boss stage started, or nothing when no
   * Arriving status came. */
  [[nodiscard]] std::optional<engine::Duration> sinceArrival(
      engine::Duration now) const;

 private:
  /** @brief Opens a lag, or adds to the open one, when @p health is less than
   * the health known. */
  void recordLoss(std::uint16_t health, engine::Duration now);

  BossFight fight_{};
  std::optional<engine::Duration> arrivedAt_;
};

}  // namespace rtype::client
