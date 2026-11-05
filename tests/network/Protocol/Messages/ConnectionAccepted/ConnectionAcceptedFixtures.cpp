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

namespace rtype::network::testing {

ConnectionAccepted acceptanceWithEnemyTypes(std::size_t count,
                                            std::string_view name) {
  ConnectionAccepted acceptance{.playerNumber = FIRST_PLAYER_BYTE, .types = {}};
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
  return payload;
}

}  // namespace rtype::network::testing
