#pragma once

#include <cstdint>
#include "ExtraLifeKeeperParts.hpp"
#include "ScoreChanged.hpp"
#include "SubscriptionHandle.hpp"

namespace rtype::server {

struct ConnectedPlayer;

/**
 * @brief Gives a player an extra life each time its score passes a step of
 * the extra lives (extraLivesWonBy()), up to identifiers::HIGHEST_LIVES, and
 * announces it with an ExtraLifeWon; a player who already holds that many
 * wins none, and nothing is announced.
 *
 * It follows the ScoreChanged the ScoreKeeper of the game publishes. Only a
 * player who still holds a life wins one: a player out of lives keeps
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
  void onScoreChanged(const game::ScoreChanged& change);
  /** @brief Gives @p player @p won lives, up to identifiers::HIGHEST_LIVES.
   * @returns Whether it holds more lives than before. */
  bool giveLives(ConnectedPlayer& player, std::uint32_t won);

  ExtraLifeKeeperParts parts_;
  engine::SubscriptionHandle subscription_;
};

}  // namespace rtype::server
