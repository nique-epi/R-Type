#pragma once

#include <cstddef>
#include <cstdint>
#include "ConnectedPlayer.hpp"
#include "GameFlowFixture.hpp"
#include "PlayedLevel.hpp"

namespace rtype::server::testing {

/**
 * @brief A game flow whose game plays a level a script directs, one of
 * LEVELS_OF_THE_FLOW_TESTS, instead of its waves.
 */
class GameFlowLevelFixture : public GameFlowFixture {
 public:
  /** @brief A game that plays the level @p level. */
  explicit GameFlowLevelFixture(PlayedLevel level);

 protected:
  /** @brief Destroys the ship of @p player and leaves it no life, as the hit
   * that takes its last life does. */
  void destroyTheLastShipOf(ConnectedPlayer& player);
  /** @brief Takes every life of the first player, FIRST_PLAYER. */
  void takeEveryLifeOfTheFirstPlayer();
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

/** @brief A game that plays `two_stages` with one continue. */
class GameFlowOneContinueFixture : public GameFlowLevelFixture {
 public:
  GameFlowOneContinueFixture();
};

/** @brief A game that plays `two_stages` without any continue. */
class GameFlowNoContinueFixture : public GameFlowLevelFixture {
 public:
  GameFlowNoContinueFixture();
};

/** @brief A game that plays `starts_at_two` from its second stage, with one
 * continue. */
class GameFlowDeclaredStartFixture : public GameFlowLevelFixture {
 public:
  GameFlowDeclaredStartFixture();
};

/** @brief A game that plays `opens_in_the_caves`. */
class GameFlowCavesFixture : public GameFlowLevelFixture {
 public:
  GameFlowCavesFixture();
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

/** @brief A game that plays `fights_a_bydo`. */
class GameFlowFightingLevelFixture : public GameFlowLevelFixture {
 public:
  GameFlowFightingLevelFixture();
};

/** @brief A game that plays `sends_a_bydo`. */
class GameFlowSendingLevelFixture : public GameFlowLevelFixture {
 public:
  GameFlowSendingLevelFixture();
};

}  // namespace rtype::server::testing
