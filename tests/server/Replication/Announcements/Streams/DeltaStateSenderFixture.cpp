#include "DeltaStateSenderFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "DeltaStateSenderTestConstants.hpp"
#include "DeltaWorldState.hpp"
#include "Entity.hpp"
#include "EntityTypeNumber.hpp"
#include "EntityTypeNumberTestConstants.hpp"
#include "NetworkIdentity.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "Position.hpp"
#include "ProtocolConstants.hpp"
#include "SentDatagrams.hpp"
#include "ServerConstants.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "StateRateMeasurementFixtures.hpp"
#include "WorldStateSender.hpp"
#include "WorldStateSenderFixture.hpp"

namespace rtype::server::testing {

DeltaStateSenderFixture::DeltaStateSenderFixture()
    : sender_(std::make_unique<WorldStateSender>(
          roster_,
          [this](network::OutgoingDatagram datagram) {
            sent_.push_back(std::move(datagram));
          },
          DEFAULT_WORLD_STATES_PER_SECOND)) {}

ConnectedPlayer& DeltaStateSenderFixture::connect(std::uint16_t port) {
  return *roster_.add(client(port), std::string{PLAYER_NICKNAME}, port);
}

engine::Entity DeltaStateSenderFixture::addEnemy(std::uint32_t networkId,
                                                 game::Position position) {
  const engine::Entity entity = entities_.create();
  components_.add(entity, game::NetworkIdentity{.networkId = networkId});
  components_.add(entity, position);
  components_.add(entity,
                  game::EntityTypeNumber{.number = DRIFTER_TYPE_NUMBER});
  return entity;
}

void DeltaStateSenderFixture::move(engine::Entity enemy,
                                   game::Position position) {
  components_.add(enemy, position);
}

void DeltaStateSenderFixture::destroy(engine::Entity enemy) {
  components_.destroy(enemy);
}

void DeltaStateSenderFixture::runTicks(std::size_t count) {
  for (std::size_t index = 0; index < count; ++index) {
    ++tick_;
    sender_->onTick(tick_, components_);
  }
}

void DeltaStateSenderFixture::acknowledgeSentTo(ConnectedPlayer& player,
                                                std::uint32_t earlier) {
  network::PacketHeader header;
  header.flags = network::ACKNOWLEDGES_FLAG;
  header.acknowledgedSequence =
      static_cast<std::uint16_t>(player.nextSequence - 1);
  header.acknowledgementBits = earlier;
  player.sentStates.acknowledge(header);
}

std::size_t DeltaStateSenderFixture::bytesOfAMeasuredRun(
    std::size_t movingEntities) {
  std::vector<engine::Entity> enemies;
  for (std::uint32_t id = 1; id <= MEASURED_ENTITIES; ++id) {
    enemies.push_back(
        addEnemy(id, game::Position{.x = static_cast<float>(id), .y = 0.0F}));
  }
  ConnectedPlayer& player = connect(FIRST_PLAYER_PORT);
  for (std::size_t tick = 1; tick <= MEASURED_TICKS; ++tick) {
    runTicks(1);
    for (std::size_t index = 0; index < movingEntities; ++index) {
      move(
          enemies.at(index),
          game::Position{.x = static_cast<float>(index + 1 + tick), .y = 0.0F});
    }
    acknowledgeSentTo(player, EVERY_EARLIER_DATAGRAM);
  }
  return bytesSentTo(sent_, client(FIRST_PLAYER_PORT));
}

const std::vector<network::OutgoingDatagram>&
DeltaStateSenderFixture::sentDatagrams() const {
  return sent_;
}

std::vector<network::DeltaWorldState> DeltaStateSenderFixture::partsSentTo(
    std::uint16_t port) const {
  std::vector<network::DeltaWorldState> parts;
  for (const network::OutgoingDatagram& datagram :
       sentTo(sent_, client(port))) {
    parts.push_back(onlyMessageIn<network::DeltaWorldState>(datagram));
  }
  return parts;
}

}  // namespace rtype::server::testing
