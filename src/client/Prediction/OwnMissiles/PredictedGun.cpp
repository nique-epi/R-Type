#include "PredictedGun.hpp"
#include <cstdint>
#include <vector>
#include "CountsDownToReady.hpp"
#include "Gun.hpp"
#include "PredictedCommand.hpp"
#include "ShipGun.hpp"

namespace rtype::client {

void PredictedGun::arm(network::ShipGun gun) {
  countDown_ = game::Gun{.intervalTicks = gun.intervalTicks,
                         .ticksUntilReady = gun.firstShotTicks,
                         .projectileNumber = gun.projectileType};
  commands_.clear();
}

bool PredictedGun::fires(const PredictedCommand& command) {
  commands_.push_back(command);
  return countsDown(command.shooting);
}

std::vector<PredictedCommand> PredictedGun::confirm(std::uint32_t firingInput) {
  countDown_.ticksUntilReady = countDown_.intervalTicks;
  std::vector<PredictedCommand> firing;
  for (const PredictedCommand& command : commands_) {
    if (command.inputNumber > firingInput && countsDown(command.shooting)) {
      firing.push_back(command);
    }
  }
  return firing;
}

void PredictedGun::forgetUpTo(std::uint32_t acknowledged) {
  while (!commands_.empty() && commands_.front().inputNumber <= acknowledged) {
    commands_.pop_front();
  }
}

bool PredictedGun::countsDown(bool shooting) {
  if (!game::countsDownToReady(countDown_) || !shooting) {
    return false;
  }
  countDown_.ticksUntilReady = countDown_.intervalTicks;
  return true;
}

}  // namespace rtype::client
