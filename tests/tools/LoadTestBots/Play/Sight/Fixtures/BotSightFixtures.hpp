#pragma once

#include <cstdint>
#include <vector>
#include "BotSightTestConstants.hpp"
#include "EntityMessage.hpp"
#include "WorldState.hpp"

namespace rtype::tools::testing {

/** @brief What a state says, for the player SIGHT_PLAYER. */
struct SightScene {
  std::uint32_t tick{SIGHT_FIRST_TICK};
  std::uint8_t lives{THREE_LIVES};
  std::uint32_t score{0};
  std::vector<network::EntityState> entities;
};

/** @returns A world state of @p scene, its ship at SHIP_X and SHIP_Y. */
[[nodiscard]] client::EntityMessage stateOf(const SightScene& scene);

/** @returns An entity of the type @p type at @p across and SHIP_Y. */
[[nodiscard]] network::EntityState entityAt(std::uint8_t type, float across);

/** @returns The end of a game. */
[[nodiscard]] client::EntityMessage endOfGame();

}  // namespace rtype::tools::testing
