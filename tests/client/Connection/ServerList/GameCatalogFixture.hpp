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
  /** @returns A server of @p serverId whose games take
   * PLAYERS_PER_GAME_OF_THE_SERVER players. */
  [[nodiscard]] static ServerRow serverOf(std::int64_t serverId);

  /** @returns A listing of game 1 in @p first and game 2 in @p second, two
   * pilots each. */
  [[nodiscard]] static network::GameListing twoGames(
      network::ListedGameState first, network::ListedGameState second);

  /** @returns A listing of game 1, waiting, played hard with five lives and
   * friendly fire on, on a server whose difficulties are easy, normal and
   * veteran. */
  [[nodiscard]] static network::GameListing aHardGame();

  /** @returns Game 1 in @p state, two pilots of four. */
  [[nodiscard]] static GameRow gameIn(network::ListedGameState state);

  [[nodiscard]] GameCatalog& catalog();
  [[nodiscard]] RecordingMasterReads& master();

 private:
  RecordingMasterReads master_;
  GameCatalog catalog_{master_};
};

}  // namespace rtype::client::testing
