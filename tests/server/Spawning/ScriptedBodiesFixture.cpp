#include "ScriptedBodiesFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "Entity.hpp"
#include "EntityDestruction.hpp"
#include "EntitySpawned.hpp"
#include "NetworkIdentity.hpp"
#include "OutgoingDatagram.hpp"
#include "Position.hpp"
#include "ScriptedBodiesParts.hpp"
#include "ScriptedSpawn.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ShippedEntitySheets.hpp"

namespace rtype::server::testing {

ScriptedBodiesFixture::ScriptedBodiesFixture()
    : bodies_(
          ScriptedBodiesParts{.world = world_,
                              .roster = roster_,
                              .send =
                                  [this](network::OutgoingDatagram datagram) {
                                    sent_.push_back(std::move(datagram));
                                  },
                              .types = game::testing::shippedEntitySheets(),
                              .events = events_,
                              .scripts = nullptr}) {
  static_cast<void>(events_.subscribe<game::EntityDestruction>(
      [this](const game::EntityDestruction& removal) {
        removals_.push_back(removal);
      }));
}

void ScriptedBodiesFixture::connectPlayers(std::size_t count) {
  for (std::size_t index = 0; index < count; ++index) {
    EXPECT_NE(roster_.add(playerEndpoint(index),
                          std::string{PLAYER_NICKNAME} + std::to_string(index),
                          SESSION_TOKEN + index),
              nullptr);
  }
}

std::uint32_t ScriptedBodiesFixture::spawnByScript(std::string_view sheet,
                                                   game::Position position) {
  return bodies_.spawn(
      game::ScriptedSpawn{.type = sheet, .position = position});
}

void ScriptedBodiesFixture::removeByScript(std::uint32_t networkId) {
  bodies_.destroy(networkId);
}

void ScriptedBodiesFixture::endTick() {
  bodies_.onTick();
  events_.dispatch();
}

std::uint32_t ScriptedBodiesFixture::placeDirectly(std::string_view sheet,
                                                   game::Position position) {
  return world_.spawn(game::testing::shippedEntitySheets().named(sheet),
                      position);
}

std::optional<game::Position> ScriptedBodiesFixture::positionOf(
    std::uint32_t networkId) {
  std::optional<game::Position> found;
  world_.components().forEach<game::NetworkIdentity, game::Position>(
      [&found, networkId](engine::Entity, const game::NetworkIdentity& identity,
                          const game::Position& position) {
        if (identity.networkId == networkId) {
          found = position;
        }
      });
  return found;
}

std::vector<network::EntitySpawned> ScriptedBodiesFixture::spawnsSentTo(
    std::size_t index) const {
  return onlyMessagesIn<network::EntitySpawned>(
      sentTo(sent_, playerEndpoint(index)));
}

const std::vector<game::EntityDestruction>& ScriptedBodiesFixture::removals()
    const {
  return removals_;
}

}  // namespace rtype::server::testing
