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
 * who fired the missile, or, for a boss, of every pilot by the damage it
 * dealt.
 *
 * Subscribes to EnemyKilled on construction and unsubscribes on destruction.
 * A kill whose damageByPilot holds any damage shares its points by that
 * damage, rounded down, the rest of the division going to the player who
 * destroyed it; a kill with no damage tallied gives all its points to that
 * player. Only a player connected under a number is credited: the share of a
 * player who left, or of a number no player holds, changes no score. The
 * bounty of a kill goes to every player connected then, whoever fired
 * (payEveryPlayer()). A
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
