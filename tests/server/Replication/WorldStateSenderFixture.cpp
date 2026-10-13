#include "WorldStateSenderFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include "Endpoint.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "NetworkIdentity.hpp"
#include "OutgoingDatagram.hpp"
#include "Position.hpp"
#include "WorldState.hpp"
#include "WorldStateCodec.hpp"

namespace rtype::server::testing {

network::Endpoint client(std::uint16_t port) {
  return network::Endpoint{.address = "127.0.0.1", .port = port};
}

network::EntityState expectedEntity(std::uint32_t networkId,
                                    network::WireEntityType type,
                                    game::Position position) {
  return network::EntityState{.entityId = networkId,
                              .entityType = type,
                              .x = position.x,
                              .y = position.y};
}

WorldStateSenderTest::WorldStateSenderTest()
    : sender_([this](network::OutgoingDatagram datagram) {
        sent_.push_back(std::move(datagram));
      }) {}

void WorldStateSenderTest::addNetworkedEntity(game::EntityType type,
                                              game::Position position,
                                              std::uint32_t networkId) {
  const engine::Entity entity = entities_.create();
  components_.add(entity, game::NetworkIdentity{.networkId = networkId});
  components_.add(entity, position);
  components_.add(entity, type);
}

void WorldStateSenderTest::addEntityWithoutNetworkId(game::EntityType type,
                                                     game::Position position) {
  const engine::Entity entity = entities_.create();
  components_.add(entity, position);
  components_.add(entity, type);
}

void WorldStateSenderTest::addEntityWithoutType(game::Position position,
                                                std::uint32_t networkId) {
  const engine::Entity entity = entities_.create();
  components_.add(entity, game::NetworkIdentity{.networkId = networkId});
  components_.add(entity, position);
}

void WorldStateSenderTest::runTicks(
    std::size_t count, const std::vector<network::Endpoint>& recipients) {
  for (std::size_t index = 0; index < count; ++index) {
    sender_.onTick(components_, recipients);
  }
}

network::WorldState WorldStateSenderTest::decodedState(
    std::size_t sentIndex) const {
  return network::WorldStateCodec::decode(sent_.at(sentIndex).payload);
}

const std::vector<network::OutgoingDatagram>&
WorldStateSenderTest::sentDatagrams() const {
  return sent_;
}

}  // namespace rtype::server::testing
