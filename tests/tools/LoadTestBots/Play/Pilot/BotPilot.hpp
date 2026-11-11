#pragma once

#include <cstdint>
#include <random>
#include "ActionSet.hpp"
#include "BotSight.hpp"
#include "IBotPilot.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools {

/**
 * @brief The keyboard of a bot: it holds one action of BOT_ACTIONS at a time,
 * for a time drawn from SHORTEST_HOLD to LONGEST_HOLD, then draws the next
 * one. Two pilots built from one seed and advanced by the same times hold the
 * same actions.
 */
class BotPilot final : public IBotPilot {
 public:
  /** @param seed The seed the actions and their times are drawn from. */
  explicit BotPilot(std::uint32_t seed);

  /**
   * @returns The action held during a frame of @p elapsed: the one drawn last,
   * until its time runs out, then a new one. The first call draws. This pilot
   * looks at nothing of @p sight.
   */
  [[nodiscard]] client::ActionSet advance(engine::Duration elapsed,
                                          const BotSight& sight) override;

 private:
  /** @brief Draws the next action and the time it is held. */
  void drawNextAction();

  std::mt19937 random_;
  client::ActionSet held_;
  engine::Duration remaining_{};
};

}  // namespace rtype::tools
