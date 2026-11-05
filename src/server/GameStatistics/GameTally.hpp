#pragma once

#include <cstdint>
#include <optional>
#include "EnemyKilled.hpp"
#include "EventBus.hpp"
#include "GameStateChanged.hpp"
#include "StatisticsSheet.hpp"
#include "SubscriptionHandle.hpp"

namespace rtype::server {

/**
 * @brief Follows the bus of one game for the statistics: numbers each game
 * that starts on it, keeps a row for each Bydo killed while it is in progress,
 * and the total when it ends.
 *
 * A Bydo killed while no game is in progress is left out: it belongs to no
 * game. A game the server stops in the middle has no end, so it leaves no
 * total. Not thread-safe: use it from the simulation thread only.
 */
class GameTally {
 public:
  /**
   * @param events The bus of the game.
   * @param sheet Where the rows go.
   * Both must outlive the tally.
   */
  GameTally(engine::EventBus& events, StatisticsSheet& sheet);

  /** @brief Subscribes to the changes of state and to the Bydos killed. */
  void follow();

  /** @brief Unsubscribes from what follow() subscribed to. */
  void forget();

 private:
  void onStateChanged(const GameStateChanged& change);
  void onEnemyKilled(const game::EnemyKilled& kill);

  engine::EventBus& events_;
  StatisticsSheet& sheet_;
  engine::SubscriptionHandle stateSubscription_{};
  engine::SubscriptionHandle killSubscription_{};
  std::optional<std::uint32_t> game_;
};

}  // namespace rtype::server
