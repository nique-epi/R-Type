#include "PredictedGunFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>
#include "PredictedCommand.hpp"
#include "PredictedGun.hpp"

namespace rtype::client::testing {

std::vector<PredictedCommand> commandsHoldingShoot(std::size_t count) {
  std::vector<PredictedCommand> commands;
  commands.reserve(count);
  for (std::size_t index = 0; index < count; ++index) {
    commands.push_back(
        PredictedCommand{.inputNumber = static_cast<std::uint32_t>(index + 1),
                         .shooting = true,
                         .shipAfter = {}});
  }
  return commands;
}

std::vector<std::uint32_t> inputsFiredBy(
    PredictedGun& gun, const std::vector<PredictedCommand>& commands) {
  std::vector<std::uint32_t> fired;
  for (const PredictedCommand& command : commands) {
    if (gun.fires(command)) {
      fired.push_back(command.inputNumber);
    }
  }
  return fired;
}

}  // namespace rtype::client::testing
