#include "PredictedGun.hpp"
#include <cstdint>
#include <vector>
#include "PredictedCommand.hpp"
#include "ShipGun.hpp"

namespace rtype::client {

void PredictedGun::arm(network::ShipGun gun) {
  gun_ = gun;
  restart();
}

void PredictedGun::restart() {
  ticksUntilReady_ = gun_.firstShotTicks;
  commands_.clear();
}

bool PredictedGun::fires(const PredictedCommand& command) {
  commands_.push_back(command);
  return countsDown(command.shooting);
}

std::vector<PredictedCommand> PredictedGun::confirm(std::uint32_t firingInput) {
  ticksUntilReady_ = gun_.intervalTicks;
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
  if (ticksUntilReady_ > 0) {
    --ticksUntilReady_;
  }
  if (ticksUntilReady_ != 0 || !shooting) {
    return false;
  }
  ticksUntilReady_ = gun_.intervalTicks;
  return true;
}

}  // namespace rtype::client
