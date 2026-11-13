#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include "ConnectedPlayer.hpp"
#include "GameFlowFixture.hpp"

namespace rtype::server::testing {

/**
 * @brief A game flow whose game plays a level a script directs, one of
 * LEVELS_OF_THE_FLOW_TESTS, instead of its waves.
 */
class GameFlowLevelFixture : public GameFlowFixture {
 public:
  /** @brief A game that plays the level @p level. */
  explicit GameFlowLevelFixture(std::string_view level);

 protected:
  /** @brief Destroys the ship of @p player and leaves it no life, as the hit
   * that takes its last life does. */
  void destroyTheLastShipOf(ConnectedPlayer& player);
  /** @returns The lives @p player has left, its ship in play included. */
  [[nodiscard]] std::uint8_t livesLeftTo(const ConnectedPlayer& player);
  /** @returns How many enemies the world of the game holds. */
  [[nodiscard]] std::size_t enemiesInPlay();
};

/** @brief A game that plays `two_stages`. */
class GameFlowTwoStagesFixture : public GameFlowLevelFixture {
 public:
  GameFlowTwoStagesFixture();
};

/** @brief A game that plays `one_stage`. */
class GameFlowOneStageFixture : public GameFlowLevelFixture {
 public:
  GameFlowOneStageFixture();
};

/** @brief A game that plays `broken`. */
class GameFlowBrokenLevelFixture : public GameFlowLevelFixture {
 public:
  GameFlowBrokenLevelFixture();
};

/** @brief A game that plays `sends_a_bydo`. */
class GameFlowSendingLevelFixture : public GameFlowLevelFixture {
 public:
  GameFlowSendingLevelFixture();
};

}  // namespace rtype::server::testing
