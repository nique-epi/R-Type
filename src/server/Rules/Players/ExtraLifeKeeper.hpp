#pragma once

#include <cstdint>
#include "EnemyKilled.hpp"
#include "ExtraLifeKeeperParts.hpp"
#include "SubscriptionHandle.hpp"

namespace rtype::server {

struct ConnectedPlayer;

/**
 * @brief Gives a player an extra life each time its score passes a step of
 * the extra lives (extraLivesWonBy()), up to identifiers::HIGHEST_LIVES, and
 * announces it with an ExtraLifeWon; a player who already holds that many
 * wins none, and nothing is announced.
 *
 * It reads the score a kill left, so it must subscribe to EnemyKilled after
 * the ScoreKeeper of the game, which adds the points first: the ScoreKeeper
 * lives as long as the game, and a keeper is built with each world, after it.
 * Only a player who still holds a life wins one: a player out of lives keeps
 * watching, whatever its missiles still in flight score. The life goes to
 * ConnectedPlayer::livesWithoutShip and to the Lives of its ship while it
 * flies one, which hold the same count.
 *
 * Subscribes on construction and unsubscribes on destruction. Not
 * thread-safe: use it from the simulation thread only.
 */
class ExtraLifeKeeper {
 public:
  explicit ExtraLifeKeeper(ExtraLifeKeeperParts parts);
  ~ExtraLifeKeeper();
  ExtraLifeKeeper(const ExtraLifeKeeper&) = delete;
  ExtraLifeKeeper& operator=(const ExtraLifeKeeper&) = delete;
  ExtraLifeKeeper(ExtraLifeKeeper&&) = delete;
  ExtraLifeKeeper& operator=(ExtraLifeKeeper&&) = delete;

 private:
  void onEnemyKilled(const game::EnemyKilled& kill);
  /** @brief Gives @p player @p won lives, up to identifiers::HIGHEST_LIVES.
   * @returns Whether it holds more lives than before. */
  bool giveLives(ConnectedPlayer& player, std::uint32_t won);

  ExtraLifeKeeperParts parts_;
  engine::SubscriptionHandle subscription_;
};

}  // namespace rtype::server
