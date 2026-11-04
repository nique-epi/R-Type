#include "WorldStateSenderKindFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "EnemyKind.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "NetworkIdentity.hpp"
#include "OutgoingDatagram.hpp"
#include "Position.hpp"
#include "SentDatagrams.hpp"
#include "ServerConstants.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "WorldState.hpp"
#include "WorldStateSenderFixture.hpp"
#include "WorldStateSenderTestConstants.hpp"

namespace rtype::server::testing {

WorldStateSenderKindFixture::WorldStateSenderKindFixture()
    : sender_(
          roster_,
          [this](network::OutgoingDatagram datagram) {
            sent_.push_back(std::move(datagram));
          },
          DEFAULT_WORLD_STATES_PER_SECOND) {
  roster_.add(client(FIRST_CLIENT_PORT), std::string{PLAYER_NICKNAME},
              SESSION_TOKEN);
}

void WorldStateSenderKindFixture::addEnemy(game::EnemyKind kind,
                                           std::uint32_t networkId) {
  const engine::Entity entity = entities_.create();
  components_.add(entity, game::NetworkIdentity{.networkId = networkId});
  components_.add(entity, ENEMY_POSITION);
  components_.add(entity, game::EntityType::Enemy);
  components_.add(entity, kind);
}

std::vector<network::EntityState>
WorldStateSenderKindFixture::listedEntities() {
  for (std::uint32_t tick = 1; sent_.empty(); ++tick) {
    sender_.onTick(tick, components_);
  }
  return onlyMessageIn<network::WorldState>(sent_.front()).entities;
}

}  // namespace rtype::server::testing
