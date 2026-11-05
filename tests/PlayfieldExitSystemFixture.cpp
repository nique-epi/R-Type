#include "PlayfieldExitSystemFixture.hpp"
#include <vector>
#include "Entity.hpp"
#include "EntityDestruction.hpp"
#include "EntityRegistry.hpp"
#include "EntityType.hpp"
#include "NetworkIdentity.hpp"
#include "PlayfieldExitSystemTestConstants.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

PlayfieldExitSystemFixture::PlayfieldExitSystemFixture() {
  events_.subscribe<EntityDestruction>(
      [this](const EntityDestruction& event) { received_.push_back(event); });
}

engine::Entity PlayfieldExitSystemFixture::add(EntityType type,
                                               float positionX) {
  const engine::Entity entity = entities_.create();
  components_.add(entity, Position{.x = positionX, .y = EXIT_ROW_Y});
  components_.add(entity, type);
  components_.add(entity, NetworkIdentity{.networkId = EXITING_NETWORK_ID});
  return entity;
}

void PlayfieldExitSystemFixture::updateAndDispatch() {
  exit_.update(components_, engine::SIMULATION_TICK_DURATION);
  events_.dispatch();
}

const engine::EntityRegistry& PlayfieldExitSystemFixture::entities() const {
  return entities_;
}

const std::vector<EntityDestruction>& PlayfieldExitSystemFixture::received()
    const {
  return received_;
}

}  // namespace rtype::game::testing
