#include "GameResultKeeperFixture.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include <optional>
#include <string>
#include "ConnectedPlayer.hpp"
#include "GameResult.hpp"
#include "GameResultKeeperTestConstants.hpp"
#include "GameRuleConstants.hpp"
#include "GameRules.hpp"
#include "GameSettings.hpp"
#include "GameState.hpp"
#include "GameStateChanged.hpp"
#include "RecordingResultsArchive.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ShippedEntitySheets.hpp"

namespace rtype::server::testing {

GameResultKeeperFixture::GameResultKeeperFixture()
    : keeper_(roster_, archive_,
              GameSettings{.waves = {},
                           .types = game::testing::shippedEntitySheets(),
                           .rules =
                               network::GameRules{
                                   .difficulty = std::string{KEEPER_DIFFICULTY},
                                   .lives = identifiers::DEFAULT_LIVES,
                                   .friendlyFire = false},
                           .vanilla = true}) {}

ConnectedPlayer& GameResultKeeperFixture::seat(std::size_t index) {
  return *roster_.add(playerEndpoint(index), std::string{NICKNAMES.at(index)},
                      SESSION_TOKEN);
}

void GameResultKeeperFixture::leave(std::size_t index) {
  const std::optional<ConnectedPlayer> departed =
      roster_.remove(playerEndpoint(index));
  ASSERT_TRUE(departed.has_value());
  keeper_.onPlayerClosing(departed.value_or(ConnectedPlayer{}));
}

void GameResultKeeperFixture::startTheGame() {
  keeper_.onStateChanged(
      GameStateChanged{.previous = GameState::WaitingForPlayers,
                       .current = GameState::Starting});
  keeper_.onStateChanged(GameStateChanged{.previous = GameState::Starting,
                                          .current = GameState::InProgress});
}

void GameResultKeeperFixture::finishTheGame() {
  keeper_.onStateChanged(GameStateChanged{.previous = GameState::InProgress,
                                          .current = GameState::Finished});
}

void GameResultKeeperFixture::waitForPlayersAgain() {
  keeper_.onStateChanged(
      GameStateChanged{.previous = GameState::Finished,
                       .current = GameState::WaitingForPlayers});
}

const RecordingResultsArchive& GameResultKeeperFixture::archive() const {
  return archive_;
}

GameResult GameResultKeeperFixture::onlyResult() const {
  EXPECT_EQ(archive_.results().size(), 1U);
  return archive_.results().empty() ? GameResult{} : archive_.results().front();
}

}  // namespace rtype::server::testing
