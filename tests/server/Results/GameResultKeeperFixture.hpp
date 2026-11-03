#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include "ConnectedPlayer.hpp"
#include "GameResult.hpp"
#include "GameResultKeeper.hpp"
#include "PlayerRoster.hpp"
#include "RecordingResultsArchive.hpp"

namespace rtype::server::testing {

/** @brief A keeper on a real roster, with an archive that records. */
class GameResultKeeperFixture : public ::testing::Test {
 protected:
  GameResultKeeperFixture();

  /** @brief Seats the player at @p index under the nickname of that index. */
  ConnectedPlayer& seat(std::size_t index);
  /** @brief Frees the seat of the player at @p index, as a session that closes.
   */
  void leave(std::size_t index);
  /** @brief Moves the game along the states of a start, tick by tick. */
  void startTheGame();
  void finishTheGame();
  void waitForPlayersAgain();

  [[nodiscard]] const RecordingResultsArchive& archive() const;
  /** @returns The only result recorded; a default one after a failure. */
  [[nodiscard]] GameResult onlyResult() const;

 private:
  PlayerRoster roster_;
  RecordingResultsArchive archive_;
  GameResultKeeper keeper_;
};

}  // namespace rtype::server::testing
