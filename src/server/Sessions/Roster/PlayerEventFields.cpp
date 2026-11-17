#include "PlayerEventFields.hpp"
#include <array>
#include <cstdint>
#include <string_view>
#include "ConnectedPlayer.hpp"
#include "EventField.hpp"
#include "ServerConstants.hpp"

namespace rtype::server {

std::array<logging::EventField, PLAYER_EVENT_FIELD_COUNT> playerEventFields(
    const ConnectedPlayer& player) {
  return {
      logging::EventField{.name = PLAYER_FIELD,
                          .value = std::int64_t{player.playerNumber}},
      logging::EventField{.name = ADDRESS_FIELD,
                          .value = std::string_view{player.endpoint.address}},
      logging::EventField{.name = PORT_FIELD,
                          .value = std::int64_t{player.endpoint.port}}};
}

}  // namespace rtype::server
