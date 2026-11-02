#pragma once

#include <cstdint>
#include <optional>
#include "Position.hpp"
#include "WorldState.hpp"

namespace rtype::client {

/** @brief What a WorldState says of the ship of one player: its entity, where
 * it is and the newest input the server received from the player. */
struct OwnShip {
  std::uint32_t shipEntityId{};
  game::Position position{};
  std::uint32_t acknowledgedInput{};
};

/**
 * @returns What @p state says of the ship of the player @p playerNumber, or
 * nothing when the state lists no such player, or no entity for its ship. A
 * state cut into several datagrams lists every player in each part and the
 * entities in one of them, so the part that does not list the ship gives
 * nothing.
 */
[[nodiscard]] std::optional<OwnShip> ownShipOf(const network::WorldState& state,
                                               std::uint8_t playerNumber);

}  // namespace rtype::client
