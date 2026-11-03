#pragma once

#include "SubscriptionHandle.hpp"

namespace rtype::engine {

class EventBus;

}  // namespace rtype::engine

namespace rtype::game {

struct EnemyKilled;

}  // namespace rtype::game

namespace rtype::server {

class PlayerRoster;

/**
 * @brief Adds the points of each enemy destroyed to the score of the player
 * who fired the missile.
 *
 * Subscribes to EnemyKilled on construction and unsubscribes on destruction.
 * Only a player connected under the number of the event is credited: a kill
 * naming a player who left, or a number no player holds, changes no score. A
 * score stops at its largest value instead of wrapping around. The bus and the
 * roster must outlive this object.
 */
class ScoreKeeper {
 public:
  ScoreKeeper(engine::EventBus& events, PlayerRoster& roster);
  ~ScoreKeeper();

  ScoreKeeper(const ScoreKeeper&) = delete;
  ScoreKeeper& operator=(const ScoreKeeper&) = delete;
  ScoreKeeper(ScoreKeeper&&) = delete;
  ScoreKeeper& operator=(ScoreKeeper&&) = delete;

 private:
  void onEnemyKilled(const game::EnemyKilled& kill);

  engine::EventBus& events_;
  PlayerRoster& roster_;
  engine::SubscriptionHandle subscription_;
};

}  // namespace rtype::server
