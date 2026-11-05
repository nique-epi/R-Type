#include "GameTally.hpp"
#include "EnemyKilled.hpp"
#include "EventBus.hpp"
#include "GameState.hpp"
#include "GameStateChanged.hpp"
#include "StatisticsSheet.hpp"

namespace rtype::server {

GameTally::GameTally(engine::EventBus& events, StatisticsSheet& sheet)
    : events_(events), sheet_(sheet) {}

void GameTally::follow() {
  stateSubscription_ = events_.subscribe<GameStateChanged>(
      [this](const GameStateChanged& change) { onStateChanged(change); });
  killSubscription_ = events_.subscribe<game::EnemyKilled>(
      [this](const game::EnemyKilled& kill) { onEnemyKilled(kill); });
}

void GameTally::forget() {
  static_cast<void>(events_.unsubscribe(stateSubscription_));
  static_cast<void>(events_.unsubscribe(killSubscription_));
}

void GameTally::onStateChanged(const GameStateChanged& change) {
  if (change.current == GameState::InProgress) {
    game_ = sheet_.openGame();
  } else if (change.previous == GameState::InProgress &&
             change.current == GameState::Finished && game_.has_value()) {
    sheet_.closeGame(*game_);
    game_.reset();
  }
}

void GameTally::onEnemyKilled(const game::EnemyKilled& kill) {
  if (game_.has_value()) {
    sheet_.addKill(*game_, kill);
  }
}

}  // namespace rtype::server
