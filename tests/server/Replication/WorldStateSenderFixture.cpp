#include "WorldStateSenderFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "Endpoint.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "NetworkConstants.hpp"
#include "NetworkIdentity.hpp"
#include "OutgoingDatagram.hpp"
#include "Position.hpp"
#include "WorldState.hpp"
#include "WorldStateCodec.hpp"

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

WorldStateSenderFixture::WorldStateSenderFixture()
    : sender_([this](network::OutgoingDatagram datagram) {
        sent_.push_back(std::move(datagram));
      }) {}

void WorldStateSenderFixture::addNetworkedEntity(game::EntityType type,
                                                 game::Position position,
                                                 std::uint32_t networkId) {
  const engine::Entity entity = entities_.create();
  components_.add(entity, game::NetworkIdentity{.networkId = networkId});
  components_.add(entity, position);
  components_.add(entity, type);
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

void WorldStateSenderFixture::runTicks(
    std::size_t count, const std::vector<network::Endpoint>& recipients) {
  for (std::size_t index = 0; index < count; ++index) {
    ++tick_;
    sender_.onTick(tick_, components_, recipients);
  }
}

network::WorldState WorldStateSenderFixture::decodedState(
    std::size_t sentIndex) const {
  return network::WorldStateCodec::decode(sent_.at(sentIndex).payload);
}

const std::vector<network::OutgoingDatagram>&
WorldStateSenderFixture::sentDatagrams() const {
  return sent_;
}

}  // namespace rtype::server::testing
