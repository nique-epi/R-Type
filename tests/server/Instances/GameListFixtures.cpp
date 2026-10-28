#include "GameListFixtures.hpp"
#include <memory>
#include "EventBus.hpp"
#include "GameInstance.hpp"
#include "GameInstanceTestConstants.hpp"
#include "GameSettings.hpp"
#include "OutgoingDatagram.hpp"
#include "WavePlanFixtures.hpp"

namespace rtype::server::testing {

std::unique_ptr<GameInstance> silentGame(engine::EventBus& events) {
  return std::make_unique<GameInstance>(
      []([[maybe_unused]] const network::OutgoingDatagram& datagram) {},
      GameSettings{.seed = SOME_SEED, .waves = testWavePlan()}, events);
}

}  // namespace rtype::server::testing
