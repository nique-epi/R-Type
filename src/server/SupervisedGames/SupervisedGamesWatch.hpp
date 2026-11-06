#pragma once

#include <functional>
#include <optional>
#include <vector>
#include "GameRegistry.hpp"
#include "HostedGame.hpp"
#include "IServerOccupancy.hpp"
#include "LatestHostedGames.hpp"
#include "LatestServerMeasures.hpp"
#include "ServerMeasures.hpp"
#include "ServerOccupancy.hpp"

namespace rtype::server {

/**
 * @brief Tells the other modules of the supervisor what its games hold: the
 * list of the master (supervisedHostedGames()) and their measures
 * (supervisedMeasures()), each published again only when it changed, and the
 * occupancy an information request is told (supervisedOccupancy()), counted
 * when asked.
 *
 * Not thread-safe, but for the two LatestValue it publishes: use it from the
 * simulation thread only.
 */
class SupervisedGamesWatch final : public IServerOccupancy {
 public:
  /** @param registry The games it reads; it must outlive the watch. */
  explicit SupervisedGamesWatch(const GameRegistry& registry);

  /** @brief Publishes the list and the measures of the games when they
   * changed since they were published last, and both the first time. */
  void publishIfChanged();

  [[nodiscard]] ServerOccupancy occupancy() const override;

  /** @returns Where the list of the master is published. */
  [[nodiscard]] LatestHostedGames& hostedGames();

  /** @returns Where the measures of the games are published. */
  [[nodiscard]] LatestServerMeasures& measures();

 private:
  std::reference_wrapper<const GameRegistry> registry_;
  LatestHostedGames hostedGames_;
  LatestServerMeasures measures_;
  std::optional<std::vector<network::HostedGame>> publishedGames_;
  std::optional<network::ServerMeasures> publishedMeasures_;
};

}  // namespace rtype::server
