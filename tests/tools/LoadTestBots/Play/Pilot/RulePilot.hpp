#pragma once

#include <cstddef>
#include "ActionSet.hpp"
#include "BotSight.hpp"
#include "IBotPilot.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools {

/**
 * @brief It fires all the time and, at each frame, tries
 * the nine ways to move: it keeps the one nothing touches over the next
 * RULE_LOOK_AHEAD_STEPS, from the speed of each enemy and missile, then the
 * one nearest the row of its target and RULE_HOME_X. Its target is the boss
 * when one is there; otherwise the enemies ahead of it are shared out between
 * the bots by their index.
 */
class RulePilot final : public IBotPilot {
 public:
  /** @param botIndex The place of the bot in its squad, from 0. */
  explicit RulePilot(std::size_t botIndex);

  [[nodiscard]] client::ActionSet advance(engine::Duration elapsed,
                                          const BotSight& sight) override;

 private:
  std::size_t botIndex_;
};

}  // namespace rtype::tools
