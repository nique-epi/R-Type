#include "ConnectionAcceptedFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include "ConnectionAccepted.hpp"
#include "ConnectionAcceptedTestConstants.hpp"
#include "EntitySide.hpp"
#include "EntityTypeEntry.hpp"
#include "ShipGun.hpp"

namespace rtype::network::testing {

ConnectionAccepted acceptanceWithEnemyTypes(std::size_t count,
                                            std::string_view name) {
  ConnectionAccepted acceptance{
      .playerNumber = FIRST_PLAYER_BYTE, .types = {}, .shipGun = {}};
  acceptance.types.assign(count, EntityTypeEntry{.side = EntitySide::Enemy,
                                                 .name = std::string{name}});
  return acceptance;
}

std::vector<std::byte> oneTypeAcceptancePayload(std::uint8_t side,
                                                std::string_view name) {
  std::vector<std::byte> payload{
      std::byte{FIRST_PLAYER_BYTE}, static_cast<std::byte>(ONE_TYPE),
      std::byte{side}, static_cast<std::byte>(name.size())};
  for (const char character : name) {
    payload.push_back(static_cast<std::byte>(character));
  }
  payload.insert(payload.end(), NO_BOX_BYTE_COUNT, std::byte{0});
  payload.insert(payload.end(), NO_GUN_BYTE_COUNT, std::byte{0});
  return payload;
}

std::vector<std::byte> acceptancePayloadOfPlayer(std::uint8_t playerNumber) {
  std::vector<std::byte> payload =
      oneTypeAcceptancePayload(ENEMY_SIDE_BYTE, SHORTEST_TYPE_NAME);
  payload.front() = std::byte{playerNumber};
  return payload;
}

ConnectionAccepted acceptanceWithGun(ShipGun gun) {
  return ConnectionAccepted{
      .playerNumber = FIRST_PLAYER_BYTE,
      .types = {EntityTypeEntry{.side = EntitySide::Player, .name = "ship"},
                EntityTypeEntry{.side = EntitySide::PlayerMissile,
                                .name = "player_missile"}},
      .shipGun = gun};
}

}  // namespace rtype::network::testing
