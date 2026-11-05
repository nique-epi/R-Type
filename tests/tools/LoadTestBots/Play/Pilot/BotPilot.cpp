#include "BotPilot.hpp"
#include <cstddef>
#include <cstdint>
#include <random>
#include "ActionSet.hpp"
#include "BotPilotConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools {

BotPilot::BotPilot(std::uint32_t seed) : random_(seed) {}

client::ActionSet BotPilot::advance(engine::Duration elapsed) {
  remaining_ -= elapsed;
  if (remaining_ <= engine::Duration::zero()) {
    drawNextAction();
  }
  return held_;
}

void BotPilot::drawNextAction() {
  std::uniform_int_distribution<std::size_t> action(0, BOT_ACTIONS.size() - 1);
  std::uniform_int_distribution<engine::Duration::rep> hold(
      SHORTEST_HOLD.count(), LONGEST_HOLD.count());
  held_.clear();
  held_.add(BOT_ACTIONS.at(action(random_)));
  remaining_ = engine::Duration{hold(random_)};
}

}  // namespace rtype::tools
