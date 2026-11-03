#include "WorldStateSenderFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "Intent.hpp"
#include "Invulnerable.hpp"
#include "Lives.hpp"
#include "NetworkConstants.hpp"
#include "NetworkIdentity.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "SentDatagrams.hpp"
#include "ServerConstants.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ShipMotion.hpp"
#include "WorldState.hpp"
#include "WorldStateSender.hpp"

namespace rtype::server::testing {

network::Endpoint client(std::uint16_t port) {
  return network::Endpoint{
      .address = std::string{network::LOOPBACK_IPV4_ADDRESS}, .port = port};
}

network::EntityState expectedEntity(std::uint32_t networkId,
                                    network::WireEntityType type,
                                    game::Position position) {
  return network::EntityState{.entityId = networkId,
                              .entityType = type,
                              .x = position.x,
                              .y = position.y};
}

WorldStateSenderFixture::WorldStateSenderFixture() {
  restartWithStatesPerSecond(DEFAULT_WORLD_STATES_PER_SECOND);
}

void WorldStateSenderFixture::restartWithStatesPerSecond(
    std::size_t statesPerSecond) {
  sender_ = std::make_unique<WorldStateSender>(
      roster_,
      [this](network::OutgoingDatagram datagram) {
        sent_.push_back(std::move(datagram));
      },
      statesPerSecond);
  sent_.clear();
  tick_ = 0;
}

ConnectedPlayer& WorldStateSenderFixture::connect(std::uint16_t port,
                                                  std::uint64_t sessionToken) {
  return *roster_.add(client(port), std::string{PLAYER_NICKNAME}, sessionToken);
}

void WorldStateSenderFixture::addNetworkedEntity(game::EntityType type,
                                                 game::Position position,
                                                 std::uint32_t networkId) {
  const engine::Entity entity = entities_.create();
  components_.add(entity, game::NetworkIdentity{.networkId = networkId});
  components_.add(entity, position);
  components_.add(entity, type);
}

void WorldStateSenderFixture::addShip(std::uint32_t networkId,
                                      game::Lives lives,
                                      game::Invulnerable protection) {
  const engine::Entity entity = entities_.create();
  components_.add(entity, game::NetworkIdentity{.networkId = networkId});
  components_.add(entity, game::ShipMotion{});
  components_.add(entity, game::Intent{});
  components_.add(entity, lives);
  components_.add(entity, protection);
}

void WorldStateSenderFixture::addEntityWithoutNetworkId(
    game::EntityType type, game::Position position) {
  const engine::Entity entity = entities_.create();
  components_.add(entity, position);
  components_.add(entity, type);
}

void WorldStateSenderFixture::addEntityWithoutType(game::Position position,
                                                   std::uint32_t networkId) {
  const engine::Entity entity = entities_.create();
  components_.add(entity, game::NetworkIdentity{.networkId = networkId});
  components_.add(entity, position);
}

void WorldStateSenderFixture::runTicks(std::size_t count) {
  for (std::size_t index = 0; index < count; ++index) {
    ++tick_;
    sender_->onTick(tick_, components_);
  }
}

network::WorldState WorldStateSenderFixture::decodedState(
    std::size_t sentIndex) const {
  return onlyMessageIn<network::WorldState>(sent_.at(sentIndex));
}

network::PacketHeader WorldStateSenderFixture::headerOf(
    std::size_t sentIndex) const {
  return network::DatagramCodec::decodeHeader(sent_.at(sentIndex).payload);
}

const std::vector<network::OutgoingDatagram>&
WorldStateSenderFixture::sentDatagrams() const {
  return sent_;
}

}  // namespace rtype::server::testing
