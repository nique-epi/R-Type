#include "GameInstanceShootingFixture.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>
#include "EntityDestruction.hpp"
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "EntitySpawned.hpp"
#include "EntityTypesTestConstants.hpp"
#include "GameInstanceFixtures.hpp"
#include "GameInstanceTestConstants.hpp"
#include "IncomingDatagram.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "Position.hpp"
#include "SentDatagrams.hpp"
#include "ShippedEntitySheets.hpp"
#include "World.hpp"

namespace rtype::server::testing {

namespace {

/** @returns The EntitySpawned of the entities of the sheet named @p sheet
 * that @p datagrams announce, oldest first; each of @p datagrams holds one
 * EntitySpawned. */
std::vector<network::EntitySpawned> spawnsOfType(
    const std::vector<network::OutgoingDatagram>& datagrams,
    std::string_view sheet) {
  const std::uint8_t type =
      game::testing::shippedEntitySheets().named(sheet).number;
  std::vector<network::EntitySpawned> spawns;
  for (const network::OutgoingDatagram& datagram : datagrams) {
    const auto spawned = onlyMessageIn<network::EntitySpawned>(datagram);
    if (spawned.entityType == type) {
      spawns.push_back(spawned);
    }
  }
  return spawns;
}

}  // namespace

void GameInstanceShootingFixture::holdForNextTick(std::size_t player,
                                                  std::uint8_t actions) {
  queue(network::IncomingDatagram{
      .sender = playerEndpoint(player),
      .payload = nextInputDatagram(player, actions, sessionHeader(player))});
}

void GameInstanceShootingFixture::runTicksDispatching(std::size_t count) {
  for (std::size_t index = 0; index < count; ++index) {
    tick();
    events().dispatch();
  }
}

std::vector<network::EntitySpawned> GameInstanceShootingFixture::missilesSentTo(
    std::size_t player) {
  std::vector<network::EntitySpawned> missiles;
  for (const network::EntitySpawned& spawned :
       spawnsOfType(ofType(network::MessageType::EntitySpawned, sentTo(player)),
                    game::testing::PLAYER_MISSILE_SHEET)) {
    if (std::ranges::none_of(missiles,
                             [&spawned](const network::EntitySpawned& kept) {
                               return kept.entityId == spawned.entityId;
                             })) {
      missiles.push_back(spawned);
    }
  }
  return missiles;
}

std::vector<network::EntitySpawned>
GameInstanceShootingFixture::enemyMissilesSentTo(std::size_t player) {
  return spawnsOfType(
      ofType(network::MessageType::EntitySpawned, sentTo(player)),
      game::testing::ENEMY_MISSILE_SHEET);
}

void GameInstanceShootingFixture::recordDestructions() {
  events().subscribe<game::EntityDestruction>(
      [this](const game::EntityDestruction& destruction) {
        destructions_.push_back(destruction);
      });
}

bool GameInstanceShootingFixture::wasDestroyed(
    std::uint32_t networkId, game::DestructionCause cause) const {
  return std::ranges::count(destructions_,
                            game::EntityDestruction{.networkId = networkId,
                                                    .cause = cause}) == 1;
}

std::uint32_t GameInstanceShootingFixture::aimAtADrifter() {
  const game::Position ship = shipPositionOf(FIRST_PLAYER);
  const std::uint32_t drifter = world().spawn(
      game::testing::shippedEntitySheets().named(game::testing::DRIFTER_SHEET),
      game::Position{.x = ship.x + BYDO_DISTANCE_AHEAD, .y = ship.y});
  holdForNextTick(FIRST_PLAYER, HOLDS_SHOOT);
  return drifter;
}

}  // namespace rtype::server::testing
