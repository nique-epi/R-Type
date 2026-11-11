#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include "BossFight.hpp"
#include "BossStatus.hpp"
#include "EntityMessage.hpp"
#include "SightFigures.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools {

/**
 * @brief What a bot keeps of its fights with the boss: for each game where the
 * boss fought, whether it was destroyed, the health it kept, and how long the
 * bot lived facing it.
 *
 * A fight starts with the first BossStatus of the stage Fighting or Enraged of
 * a game and ends with its GameEnded. The Arriving stage is not a fight: the
 * boss does not attack yet. A fight still running when the report is taken is
 * not in it.
 */
class BossTally {
 public:
  /** @brief Follows the boss through @p message when it is a BossStatus, and
   * closes the fight when it is the end of the game. */
  void count(const client::EntityMessage& message);

  /**
   * @brief Adds @p elapsed to the time alive while the boss fights and the bot
   * still has a life, and the lives @p figures says it lost since the previous
   * call.
   * @param figures What the bot knows of its ship and its run; its lives lost
   * only grow.
   */
  void addTime(engine::Duration elapsed, const SightFigures& figures);

  /** @returns The fights that ended, in the order they did. */
  [[nodiscard]] std::vector<BossFight> fights() const;

 private:
  void readStatus(const network::BossStatus& status);
  void endFight();

  std::optional<BossFight> current_;
  /** @brief Whether the boss of the game in progress fights now: from its
   * first Fighting status to its Defeated one. */
  bool bossFights_{false};
  std::vector<BossFight> ended_;
  /** @brief The lives lost the previous addTime() read. */
  std::uint32_t previousLivesLost_{0};
};

}  // namespace rtype::tools
