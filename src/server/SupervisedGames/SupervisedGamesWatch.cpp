#include "SupervisedGamesWatch.hpp"
#include <utility>
#include <vector>
#include "GameRegistry.hpp"
#include "HostedGame.hpp"
#include "LatestHostedGames.hpp"
#include "LatestServerMeasures.hpp"
#include "ServerMeasures.hpp"
#include "ServerOccupancy.hpp"
#include "SupervisedGamesReport.hpp"

namespace rtype::server {

SupervisedGamesWatch::SupervisedGamesWatch(const GameRegistry& registry)
    : registry_(registry) {}

void SupervisedGamesWatch::publishIfChanged() {
  std::vector<network::HostedGame> games =
      supervisedHostedGames(registry_.get().all());
  if (games != publishedGames_) {
    publishedGames_ = games;
    hostedGames_.publish(std::move(games));
  }
  const network::ServerMeasures measures =
      supervisedMeasures(registry_.get().all());
  if (measures != publishedMeasures_) {
    publishedMeasures_ = measures;
    measures_.publish(measures);
  }
}

ServerOccupancy SupervisedGamesWatch::occupancy() const {
  return supervisedOccupancy(registry_.get().all());
}

LatestHostedGames& SupervisedGamesWatch::hostedGames() { return hostedGames_; }

LatestServerMeasures& SupervisedGamesWatch::measures() { return measures_; }

}  // namespace rtype::server
