#include "BotPilotFixtures.hpp"
#include <cstddef>
#include <vector>
#include "ActionSet.hpp"
#include "BotPilot.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools::testing {

std::vector<client::ActionSet> heldOver(BotPilot& pilot, std::size_t frames) {
  std::vector<client::ActionSet> held;
  held.reserve(frames);
  for (std::size_t frame = 0; frame < frames; ++frame) {
    held.push_back(pilot.advance(engine::SIMULATION_TICK_DURATION));
  }
  return held;
}

}  // namespace rtype::tools::testing
