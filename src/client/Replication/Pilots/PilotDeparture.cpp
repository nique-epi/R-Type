#include "PilotDeparture.hpp"
#include "PilotStatus.hpp"

namespace rtype::client {

bool hasLeft(const PilotStatus& pilot) {
  return pilot.state == PilotState::Left ||
         pilot.state == PilotState::SignalLost ||
         pilot.state == PilotState::Expelled ||
         pilot.state == PilotState::Banned;
}

}  // namespace rtype::client
