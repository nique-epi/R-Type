#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include "GameCatalog.hpp"
#include "GameListing.hpp"
#include "GameRow.hpp"
#include "ListedGameState.hpp"
#include "RecordingMasterReads.hpp"
#include "ServerRow.hpp"

namespace rtype::client::testing {

/** @brief A catalog of games over a master the tests answer for. */
class GameCatalogFixture : public ::testing::Test {
 protected:
  /** @returns A server of @p id whose games take
   * PLAYERS_PER_GAME_OF_THE_SERVER players. */
  [[nodiscard]] static ServerRow serverOf(std::int64_t id);

  /** @returns A listing of game 1 in @p first and game 2 in @p second, two
   * pilots each. */
  [[nodiscard]] static network::GameListing twoGames(
      network::ListedGameState first, network::ListedGameState second);

  /** @returns Game 1 in @p state, two pilots of four. */
  [[nodiscard]] static GameRow gameIn(network::ListedGameState state);

  [[nodiscard]] GameCatalog& catalog();
  [[nodiscard]] RecordingMasterReads& master();

 private:
  RecordingMasterReads master_;
  GameCatalog catalog_{master_};
};

}  // namespace rtype::client::testing
